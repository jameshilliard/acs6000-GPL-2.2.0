# Input is in the form of:
#
# cn_tree = {
# 	'bootconf': {
# 		'wdt': {
# 			'get:': '',
# 			'set:': '',
# 			'sanity:': 'or(equal($_, \'yes\'), equal($_, \'no\'))',
# 			'help:': '\'enable/disable watchdog timer (yes/no)\''
# 		},
# 		'ip': {
# 			'get:': '',
# 			'set:': '',
# 			'sanity:': 'isip($_)',
# 			'help:': '\'initial ip address\''
# 		},
#
# That data structure is maintained unaltered in memory.
#
# Wherever there's a variable named "nodes", it describes a path from the root
# node down to a target node.  The list contains tuples of <name, node>, where
# "name" is the key name in the parameter tree (i.e. 'bootconf', 'wdt'), and
# "node" is a reference to the "map" in the actual tree.
#
# "args" is a standard dictionary with keys 'type', 'name', 'direction' and
# 'description'.  This is used to generate function prototypes, doc comments
# and function calls.

import re, parameter_tree, management_tree


# Convert a parameter-tree name into something suitable for use as part of a
# C function name (with no underscores).
# i.e. "power-supply" => "powerSupply"
#
# @param [in] tree_component The path component to convert
# @return The path component in a format suitable for use as a C identifier.
def c_path_component(tree_component):
	if tree_component == '*':
		return tree_component

	return re.sub('[-_]([a-zA-Z0-9])', lambda match: match.group(1).upper(),
		re.sub('[^-a-zA-Z0-9_]', '', tree_component))

# Return the results of executing the given function on each node along the
# given path.  The function should accept a tuple of <name, node>.
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @param [in] fn The function to execute for each (name,node) pair.
# @return The list of results of executing fn on the nodes.
def traverse_down_to(nodes,fn):
	return map(lambda x:fn(x), nodes[1:])

# Returns a C "printf" format string into which the wildcard names may be
# substituted.
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @return The format string.
def printf_string(nodes):
	return ".".join(traverse_down_to(nodes, lambda x:x[0]=="*" and '%s' or x[0]))

# Returns a name for this node's path, suitable for use as part of a C function
# name, e.g. "network_interface_star0_address"
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @return The path, in a format suitable for use as a C identifier.
def c_path(nodes):
	name_components = traverse_down_to(nodes, lambda x:c_path_component(x[0]))
	star_count = 0
	for i in range(len(name_components)):
		if name_components[i]=="*":
			name_components[i] = "star" + str(star_count)
			star_count = star_count + 1

	return "_".join(name_components)

# Returns the name for this node, as seen by CACPD (including wildcards).
# e.g. "network.interface.*.address"
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @return The CACPD address for the given node.
def cacpd_path(nodes):
	return ".".join(traverse_down_to(nodes, lambda x:x[0]))

# Returns a list of (name,node) tuples, representing only the wildcard nodes,
# in order, from the root down to this node.
# e.g. for network.interface.*.alias.*.address, will return:
#    [("*", child-of-interface), ("*", child-of-alias)]
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @return A map of (n,node) representing the wildcards from the root down
#	to here.
def find_stars(nodes):
	# Remember that "type:" is specified in the PARENT of "*", hence +1/-1.
	star_node_indices = filter(lambda n:nodes[n+1][0]=="*", range(len(nodes)-1))
	star_nodes = map(lambda n:nodes[n], star_node_indices)
	return map(lambda n:(n,star_nodes[n][1]), range(len(star_nodes)))

# Return the results of executing the given function on each wildcard node
# along the given path.  The function should accept a tuple of (n, node).
#
# @param [in] nodes The set of (n,node) tuples describing the path to this
#		node.
def traverse_stars_down_to(nodes,fn):
	stars = find_stars(nodes)
	return map(lambda n:fn(n,stars[n][1]), range(len(stars)))

# Returns true iff this path contains at least one wildcard -- then we know we
# need to generate (at runtime) the path to send to CACPD.
#
# @param [in] nodes The set of (name,node) tuples describing the path to this
#		node.
# @return True iff this path contains at least one wildcard.
def has_wildcards(nodes):
	return len(find_stars(nodes)) > 0

# Returns the parameter-tree type of the values in the given node.
#
# @param [in] node The node of interest.
# @return A string containing the types.conf name of the values at this node.
def type_for_node(node):
	if node.has_key("type:"):
		return node["type:"]
	return "UTF8String"


def free_msg(paramtype):
	return "; FIXME: Document properly how to free this.  The client *may* need to free the result using paramtype_%s_free(); simpler types (UTF8String) may be freed using just free(); primitive types (boolean, int) are placed directly into storage provided by the caller, and don't need to be freed."

# Returns True iff the given node is within the management tree, False otherwise.
#
# @param [in] nodes The node of interest.
# @return True iff the given node is within the management tree.
def node_is_management(nodes):
	return nodes[0][1] == management_tree.cn_tree

# Returns True iff the given node's type doesn't DIRECTLY equate to a char *.
# When this function returns true, a conversion will be needed to/from the
# appropriate type.
#
# @param [in] node The node of interest.
# @return True iff the node's native C type doesn't directly equate to "char *".
def needs_TS_conversion(node):
	return (type_for_node(node)!="UTF8String")

# For each wildcard in the list of nodes from the root down to the node of
# interest, execute a given function and return a list of the results.
#
# @param [in] fn The function to execute.  Receives two arguments: the index
#	in the node list of "this" wildcard, and a reference to the wildcard
#	node.
# @param [in] nodes The path to the node of interest.
# @return A list of the results of running "fn" on each wildcard.
def traverse_stars_needing_TS_conversion(nodes,fn):
	stars = find_stars(nodes)
	stars_needing_conversion = filter(lambda t:needs_TS_conversion(t[1]), stars)
	return map(lambda t:fn(t[0],t[1]), stars_needing_conversion)

# Obtains the native C type to be used to represent the node of interest.
#
# @param [in] node The node of interest.
# @return A string containing the C type to use for values at this node.
def c_type_for_node(node):
	if node.has_key("type:"):
		return "paramtype_" + node["type:"] # FIXME: Look this up in types.conf to get better C type names?
	return "char *"

# Obtains the name of the struct type for representing lists of items at this
# node.
#
# @param [in] node The node of interest.
# @return A string containing the C type to use for lists of values at this
#	node.
def list_type_for_node(node):
	return "paramtype_" + type_for_node(node) + "_list"

# Outputs the given function prototype.
#
# @param [in] comment A list of lines representing the comment to attach to the
#	function in the output.
# @param [in] prototype A single line of text containing the function prototype.
# @param [in] body A list of lines for the function body.
# @param [in] function_is_public True iff the function should appear in the
#	header; False omits from the header.
def output_function(comment, prototype, body, function_is_public):
	global header_file, source_file
	if function_is_public:
		# Write the doc comment to the header.
		if len(comment) > 0:
			header_file.write("/**\n * " + "\n * ".join(comment) + "\n */\n")
		header_file.write(prototype+";\n\n")
	else:
		if len(comment) > 0:
			source_file.write("/**\n * " + "\n * ".join(comment) + "\n */\n")

	source_file.write(prototype+"\n")
	source_file.write("{\n\t" + "\n\t".join(body) + "\n}\n\n")

# Given a single line of text, line-wrap it if necessary.
#
# @param [in] line The source line of text (possibly long).
# @return A list of lines, which are a wrapped as appropriate.
def wrap_comment_line(line):
	words = line.split(' ')
	result = ['']
	indent = 0
	prefix = re.compile("^@(return|param \[[^]]*\] [^ ]*) ").match(line)
	if (prefix != None):
		indent = 3 # prefix.end()

	for w in words:
		if len(result[-1]) + len(w) > 77:
			# Start a new line
			result.append(' ' * indent)
		result[-1] = result[-1] + ' ' + w
	
	return result

# Produce a set of lines containing a documentation comment for the given
#	function specification.
#
# @paran [in] description A string description of the function.
# @param [in] args The description of the arguments to the function, each as
#	a dictionary with keys 'name', 'type', 'direction' and
#	'description'.
# @param [in] returns A string describing the result of the function.
# @return A list of strings containing the generated doc comment.
# @note This doc comment may or may not have been automatically generated.
def make_doc_comment(description, args, returns):
	doc_comment = [description, ''] + \
		map(lambda arg:'@param [%s] %s %s' % (arg["direction"], arg["name"], arg["description"]), args);
	if len(returns) > 0:
		doc_comment.append("@return " + returns);

	# Do line-wrapping on the comment text:
	wrapped_doc_comment = []
	for comment_line in doc_comment:
		wrapped_doc_comment = wrapped_doc_comment + wrap_comment_line(comment_line)
	return wrapped_doc_comment

# Obtain the name of the path-generation function which can be called at run-
# time to generate the string with the CACPD path to a given node.
#
# @param [in] nodes The path to the node of interest.
# @return A string containing the name of the node.
def pathgen_function_name_for(nodes):
	return (node_is_management(nodes) and 'mgmt' or 'param') + '_' + c_path(nodes)

# Outputs a path-generation function (which can be called at runtime to
# generate a string representation of a CACPD path) for a particular node.
#
# @param [in] nodes The path to the node of interest.
def generate_pathgen_function(nodes):
	global pathgens_already_written
	pathgen_func = pathgen_function_name_for(nodes)
	if pathgen_func in pathgens_already_written:
		return

	comment = make_doc_comment("Produces the CACPD tree path for accessing the node \"" + cacpd_path(nodes) + "\"",
		traverse_stars_down_to(nodes, 
			lambda n,node:{'name':'star'+str(n), 'direction':'in', 'description':'The text of replaceable parameter #'+str(n)}),
		"A malloc()ed string with the path to the requested node.  Substituted components will be suitably escaped."
	)

	output_function(comment, \
		"static char *gsp_psapi_pathgen_" + pathgen_func + "(" + ", ".join(traverse_stars_down_to(nodes, lambda n,node:'const char *star'+str(n))) + ")",
		["char *result = NULL;",
		"",
		"/* Convert each wildcard to a string, ready for substitution in the address. */"] + \
		traverse_stars_down_to(nodes, lambda n,node:'char *star'+str(n)+'_escaped = url_encode(star'+str(n)+');') +
		["",
		"/* Substitute into the skeleton of the address. */",
		"if (asprintf(&result, \"" + printf_string(nodes) + "\", " + ", ".join(traverse_stars_down_to(nodes, lambda n,node:'star'+str(n)+'_escaped')) + ") < 0)",
		"\tresult = NULL;",
		"",
		"/* Free the memory used for the conversions. */"] + \
		traverse_stars_down_to(nodes, lambda n,node:'free(star'+str(n)+'_escaped);') + \
		["",
		"return result;"], False)

	pathgens_already_written.append(pathgen_func)

# Outputs a path-safe API function.  The PSAPI functions are similar enough
# that they may be generated from one function, although the logic is a bit
# twisted.
#
# @param [in] nodes The path to the node for which we're generating the PSAPI.
# @param [in] action The action that this function will represent.  Should be
#	one of 'get', 'set', 'add', 'del', 'list', 'move', 'admin', 'info'.
# @param [in] additional_args Additional arguments that need to be passed into
#	the generated function at runtime -- i.e. the value that's going to be
#	set or added, or storage for a returned value.  See the comment at the
#	top of this file for a description.
# @param [in] send_command The code that will send the request off to CACPD.
# @param [in] handle_response A list of strings containing code to handle the
#	server's response.
def generate_psapi_function(nodes, action, additional_args, send_command, handle_response):
	# If necessary, generate the CACPD path.
	log_address_components = [
		"/* Log the request inputs */",
		"dlog_printf(DLOG_DEBUG, \"PSAPI\", \"Invoked PSAPI function \\\"%s\\\" for CACPD path \\\"%s\\\".\", __func__, \"" + cacpd_path(nodes) + "\");"]
	if has_wildcards(nodes):
		generate_pathgen_function(nodes)
		address_create = ["", "/* Ask the path generation function to make the path */",
			"char *cacpd_path = gsp_psapi_pathgen_" + pathgen_function_name_for(nodes) + 
			"(" + ", ".join(traverse_stars_down_to(nodes, lambda n,node:'star'+str(n))) + ");"]
		address_destroy = ["", "/* We're done with the address. */", "free(cacpd_path);"]
		log_address_components.append(
			"dlog_printf(DLOG_DEBUG, \"PSAPI\", \"   " + ", ".join(traverse_stars_down_to(nodes, lambda n,node:'star'+str(n)+'=\\\"%s\\\"')) + "\", " +
			", ".join(traverse_stars_down_to(nodes, lambda n,node:'star'+str(n))) + ");"
		)
	else:
		address_create = ["", "char *cacpd_path = \"" + cacpd_path(nodes) + "\";"]
		address_destroy = []

	# Generate the arguments for the function
	sess = {"type":"gsp_tsapi_session_t *", "name":"sess", "direction":"in", "description":"The GSP TSAPI session handle."}

	args = [sess] + \
		additional_args + \
		traverse_stars_down_to(nodes, \
			lambda n,node:{'name':'star'+str(n), 'type':'const char *', 'direction':'in', 'description':'The text of replaceable parameter #'+str(n)})

	# Generate the doc comment for this PSAPI function
	comment = make_doc_comment("Performs the \"" + action + "\" action at CACPD node \"" + cacpd_path(nodes) + "\" with" +
		" character strings as input and output.", args, \
		"A typesafe API result code (TS_OK, CONNECT_FAIL, COMMAND_FAIL).")

	# The prototype for this PSAPI function:
	prototype = "int gsp_psapi_" + action + "_" + c_path(nodes) + \
		"(" + ", ".join(map(lambda arg:arg['type'] + ' ' + arg['name'], args)) + ")"

	# Hold on, here comes the function body!
	body = log_address_components + \
		["",
		"/* Reconnect to CACPD */",
		"int result = tsapi_make_session_ready(sess);",
		"if (TS_OK != result){",
		"	dlog_printf(DLOG_WARNING, \"PSAPI\", \"Unable to make TSAPI session ready for use.\");",
		"	return result;",
		"}",
		] + \
		address_create + \
		["",
		"dlog_printf(DLOG_DEBUG, \"PSAPI\", \"Request to CACPD will use path: \\\"%s\\\".\", cacpd_path);",
		"",
		"/* Send the request. */"
		"cacpdclient_transaction *trans;"] + \
		send_command + \
		address_destroy + \
		["",
		"if (!trans) {"
		"	dlog_printf(DLOG_WARNING, \"PSAPI\", \"Unable to send request to CACPD.\");",
		"	return CONNECT_FAIL;"
		"}",
		"",
		"/* Handle the response. */",
		"if (CACP_RESULT_OK != cacpdclient_transaction_get_result_code(trans)) {",
		"	result = COMMAND_FAIL;",
		"	dlog_printf(DLOG_NOTICE, \"PSAPI\", \"An error was encountered during the PSAPI request.\");",
		"}" + (len(handle_response) > 0 and " else {" or "")] + \
		(len(handle_response) > 0 and ['\tresult = TS_OK;'] or []) + \
		map(lambda x:'\t'+x, handle_response) + \
		(len(handle_response) > 0 and ["}"] or []) + \
		["",
		"/* Disconnect from CACPD. */",
		"int disconnect_result = tsapi_make_session_idle(sess);",
		"if (TS_OK != disconnect_result) {",
		"	dlog_printf(DLOG_WARNING, \"PSAPI\", \"Unable to make TSAPI session idle.\");",
		"}",
		"",
		"dlog_printf(DLOG_DEBUG, \"PSAPI\", \"PSAPI function completed %ssuccessfully.\", (TS_OK == result ? \"\" : \"un\"));",
		"",
		"/* Discard the response. */",
		"cacpdclient_destroy_transaction(trans);",
		""] + \
		["return result;"]

	# Send it to the output files.
	output_function(comment, prototype, body, True)

# Outputs a type-safe API function.  The TSAPI functions are similar enough
# that they may be generated from one function, although the logic is a bit
# twisted.  They do the type-conversions, and depend on the PSAPI for all
# further processing.
#
# @param [in] nodes The path to the node for which we're generating the TSAPI.
# @param [in] action The action that this function will represent.  Should be
#	one of 'get', 'set', 'add', 'del', 'move', 'admin', 'info'.
# @param [in] additional_args Additional arguments that need to be passed into
#	the generated function at runtime -- i.e. the value that's going to be
#	set or added, or storage for a returned value.  See the comment at the
#	top of this file for a description.
# @param [in] prepare_request A list of strings containing code to prepare for
#	sending the request to the PSAPI.
# @param [in] args_to_psapi A list of strings representing any extra arguments
#	to pass to the Path-Safe API, i.e. ["foo", "&response"].
# @param [in] handle_response A list of strings containing code to handle the
#	PSAPI response.
def generate_tsapi_function(nodes, action, additional_args, prepare_request, args_to_psapi, handle_response, clean_up):
	# If necessary, perform type-translation on the arguments.
	star_translate = traverse_stars_needing_TS_conversion(nodes, lambda n,node: \
		'char *star' + str(n) + '_cstr = paramtype_' + type_for_node(node) + '_fromC(star' + str(n) + ');')

	if len(star_translate) > 0:
		star_translate[0:0]=["/* Perform type conversion on the wildcard arguments. */"]
		star_translate.append("")

	star_destroy = traverse_stars_needing_TS_conversion(nodes, lambda n,node:'free(star' + str(n) + '_cstr);')
	if len(star_destroy) > 0:
		star_destroy[0:0]=["/* Clean up the temporaries from type-conversion of wildcards. */"]
		star_destroy.append("")

	# Generate the arguments for the function
	sess = {"type":"gsp_tsapi_session_t *", "name":"sess", "direction":"in", "description":"The GSP TSAPI session handle."}

	args = [sess] + \
		additional_args + \
		traverse_stars_down_to(nodes, \
			lambda n,node:{'name':'star'+str(n), 'type':'const ' + c_type_for_node(node), 'direction':'in', \
			'description':'The ' + type_for_node(node) + ' value of replaceable parameter #'+str(n)})
		#traverse_stars_down_to(nodes, lambda n,node:'const '+c_type_for_node(node)+' star'+str(n))

	# Generate the doc comment for this TSAPI function
	comment = make_doc_comment("Performs the \"" + action + "\" action at CACPD node \"" + cacpd_path(nodes) + "\" with type-safe " +
		"input and output.", args, \
		"A typesafe API result code (TS_OK, CONNECT_FAIL, COMMAND_FAIL, TS_CONVERSION_FAILURE).")

	# The prototype for this TSAPI function:
	prototype = "int gsp_tsapi_" + action + "_" + c_path(nodes) + \
		"(" + ", ".join(map(lambda arg:arg['type'] + ' ' + arg['name'], args)) + ")"

	if len(handle_response) > 0:
		handle_response = [ \
			"/* Check the result of the request. */",
			"if (TS_OK == result){"] + \
			map(lambda x:'\t'+x, handle_response) + \
			["}", ""]

	# Assemble the body of the function.
	body = ["/* Log the TSAPI request. */",
		"dlog_printf(DLOG_DEBUG, \"TSAPI\", \"Invoked TSAPI function \\\"%s\\\" for CACPD path \\\"%s\\\".\", __func__, \"" + cacpd_path(nodes) + "\");"] + \
		star_translate + \
		prepare_request + \
		(len(prepare_request) and [""] or []) + \
		["/* Pass the request to the path-safe API for handling. */",
		"dlog_printf(DLOG_DEBUG, \"TSAPI\", \"Passing request to the PSAPI.\");",
		"int result = gsp_psapi_" + action + "_" + c_path(nodes) + "(" +
			", " . join(
				["sess"] + 
				args_to_psapi +
				traverse_stars_down_to(nodes, lambda n,node:'star' + str(n) + (needs_TS_conversion(node) and '_cstr' or ''))
			) + ");",
		""] + \
		star_destroy + \
		handle_response + \
		clean_up + \
		["/* Log the result of the TSAPI request. */", 
		"dlog_printf(DLOG_DEBUG, \"TSAPI\", \"TSAPI function completed %ssuccessfully.\", (TS_OK == result ? \"\" : \"un\"));",
		"return result;"]

	# Send it to the output files.
	output_function(comment, prototype, body, True)

# Generate all the PSAPI and TSAPI code to handle a particular action (get,
# set, etc...) at a given node.
# @param [in] nodes The path to the node of interest.
# @param [in] action A string containing the action for which we want to
#	generate the code.  Should be 'get', 'set', 'add, 'del', 'list',
#	'move', 'admin' or 'info'.
def handle_action(nodes, action):
	# In the last element of "nodes", the second element of the tuple is
	# the node we're currently examining.
	node = nodes[-1][1]
	nodetype = type_for_node(node)
	conn = 'gsp_tsapi_get_connection(sess)'

	if "set"==action or "add"==action or "del"==action:
		#
		# Pathsafe API
		#
		if action=='set':
			description='The new value to set into this node.'
		elif action=='add':
			description='The name of the new node to add.'
		elif action=='del':
			description='The name of the node to delete.'

		generate_psapi_function(nodes, action,
			[{'type':'const char *','name':'value','direction':'in','description':description}],
			["trans = cacpdclient_send_" + action + "(" + conn + ", cacpd_path, value);"], [])

		#
		# Typesafe API
		#
		if action=='set':
			description='The new %s value to set into this node.' % nodetype
		elif action=='add':
			description='The %s value of the new node to add.' % nodetype
		elif action=='del':
			description='The %s value of the node to delete.' % nodetype

		if (needs_TS_conversion(node)):
			convert_value = \
				["/* Convert the new native value into a string. */",
				"char *value = paramtype_" + type_for_node(node) + "_fromC(typesafe_value);"]
			destroy_value = \
				["/* We're done with the converted value. */",
				"free(value);",
				""]
		else:
			convert_value = ["const char *value = typesafe_value;"]
			destroy_value = []

		generate_tsapi_function(nodes, action,
			[{'type':'const ' + c_type_for_node(node),'name':'typesafe_value','direction':'in','description':description}],
			convert_value, ["value"], [], destroy_value)
	
	if "move"==action:
		generate_psapi_function(nodes, action,
			[{'type':'const char *', 'name':'old_name', 'direction':'in', 'description':'The old name of the child node to rename.'},
			{'type':'const char *', 'name':'new_name', 'direction':'in', 'description':'The new name for the child node.'}],
			["trans = cacpdclient_send_" + action + "(" + conn + ", cacpd_path, old_name, new_name);"], [])

		if (needs_TS_conversion(node)):
			convert_names = \
				["/* Convert the old and new values into a string. */",
				"char *old_name = paramtype_" + nodetype + "_fromC(old_value);",
				"char *new_name = paramtype_" + nodetype + "_fromC(new_value);"]
			destroy_names = \
				["/* We're done with the converted values. */",
				"free(old_name);",
				"free(new_name);",
				""]
		else:
			convert_names = ["const char *old_name = old_value;",
				"const char *new_name = new_value;"]
			destroy_names = []

		generate_tsapi_function(nodes, action,
			[{'type':'const ' + c_type_for_node(node), 'name':'old_value','direction':'in','description':'The old value of the child node to change.'},
			{'type':'const ' + c_type_for_node(node), 'name':'new_value','direction':'in','description':'The new value of the child node.'}],
			convert_names, ["old_name", "new_name"], [], destroy_names)

	if "get"==action:
		#
		# Pathsafe API
		#
		generate_psapi_function(nodes, action,
			[{"type":"char **", "name":"response", "direction":"out", "description":"Receives the value from CACPD; only valid if return is TS_OK; caller must free."}],
			["trans = cacpdclient_send_" + action + "(" + conn + ", cacpd_path);"],
			["*response = strdup(cacpdclient_transaction_get_value(trans));"])

		#
		# Typesafe API
		#
		if (needs_TS_conversion(node)):
			convert_response = \
				["/* Convert the string value into a native type. */",
				"int conversion_result = paramtype_" + type_for_node(node) + "_toC(response, typesafe_response);",
				"free(response);",
				"if (!conversion_result)",
				"\tresult = TS_CONVERSION_FAILURE;"]
		else:
			convert_response = ["*typesafe_response = response;"]
		generate_tsapi_function(nodes, action,
			[{'type':c_type_for_node(node) + ' *','name':'typesafe_response','direction':'out','description':'Receives the typesafe %s value from CACPD; only valid if return is TS_OK%s.' % (nodetype, free_msg(nodetype))}],
			["char *response = NULL;"], ["&response"], convert_response, [])

	if "list"==action:
		#
		# Pathsafe API
		#
		if node_is_management(nodes):
			action = "mlist"

		generate_psapi_function(nodes, action,
			[{"type":"struct paramtype_UTF8String_list **", "name":"response", "direction":"out", "description":"Receives for the head of the list of retrieved values; only valid if return is TS_OK; caller must free using paramtype_UTF8String_list_free()."}],
			["trans = cacpdclient_send_" + action + "(" + conn + ", cacpd_path);"],
			["*response = cacpdclient_list_to_UTF8String_list(cacpdclient_transaction_get_list(trans));"])

		#
		# Typesafe API
		#
		if (needs_TS_conversion(node)):
			convert_response = \
				["/* Convert the list of strings into a list of a native type. */",
				"int list_conversion_result = " + list_type_for_node(node) + "_toC(response, typesafe_response);",
				"paramtype_UTF8String_list_free(response);",
				"if (list_conversion_result)",
				"\tresult = TS_CONVERSION_FAILURE;"]
		else:
			convert_response = ["*typesafe_response = response;"]
		generate_tsapi_function(nodes, action,
			[{'type':'struct ' + list_type_for_node(node) + ' **','name':'typesafe_response','direction':'out', 'description':'Receives the head of the list of retrieved %s values; caller must free using paramtype_%s_list_free().' % (nodetype, nodetype)}],
			["struct paramtype_UTF8String_list *response = NULL;"], ["&response"], convert_response, [])

	if "admin"==action:
		#
		# Pathsafe API
		#
		generate_psapi_function(nodes, action,
			[{"type":'const char * const *',"name":'args', "direction":"in", "description":"A NULL-terminated list of arguments to pass to the \"ADMIN\" function, or just NULL if no arguments are to be passed."}],
			["trans = cacpdclient_send_admin(" + conn + ", cacpd_path, args);"], [])

		#
		# Typesafe API
		#
		generate_tsapi_function(nodes, action,
			[{'type':'const char * const *','name':'args','direction':'in','description':'A NULL-terminated list of arguments to pass to the \"ADMIN\" function, or just NULL if no arguments are to be passed.'}], [], ["args"], [], [])

	if "info"==action:
		#
		# Pathsafe API
		#
		generate_psapi_function(nodes, action,
			[{"type":'char **', "name":'response', "direction":"out", "description":"Receives for the string result of the \"INFO\" request; only valid if return is TS_OK; caller must free."},
			{"type":'const char * const *',"name":'args', "direction":"in", "description":"A NULL-terminated list of arguments to pass to the \"INFO\" function, or just NULL if no arugments are to be passed."}],
			["trans = cacpdclient_send_" + action + "(" + conn + ", cacpd_path, args);"],
			["*response = strdup(cacpdclient_transaction_get_value(trans));"])

		#
		# Typesafe API
		#
		if (needs_TS_conversion(node)):
			convert_response = \
				["/* Convert the string value into a native type. */",
				"int conversion_result = paramtype_" + type_for_node(node) + "_toC(response, typesafe_response);",
				"free(response);",
				"if (!conversion_result)",
				"\tresult = TS_CONVERSION_FAILURE;"]
		else:
			convert_response = ["*typesafe_response = response;"]
		generate_tsapi_function(nodes, action,
			[{'type':c_type_for_node(node) + ' *','name':'typesafe_response','direction':'out','description':'Receives the type-safe %s result of the \"INFO\" request; only valid if return is TS_OK%s.' % (nodetype, free_msg(nodetype))},
			{"type":'const char * const *',"name":'args', "direction":"in", "description":"A NULL-terminated list of arguments to pass to the \"INFO\" function, or just NULL if no arguments are to be passed."}],
			["char *response = NULL;"], ['&response', 'args'], convert_response, [])


# A recursive tree-walker.
# @param [in] nodes The path from the root down to the node we're currently
#	visiting.
def traverse_tree(nodes):
	global header_file, source_file, header_files
	name,node = nodes[-1]

	#handle_node(nodes)

	# Look through the children under "node".
	children = []
	for child_name in node.keys():
		if child_name in ["get:", "set:", "add:", "del:", "move:", "list:", "info:", "admin:"]:
			# This is an action for which we write code.
			handle_action(nodes, child_name[:-1])
		elif not child_name in ["type:", "len:", "sanity:", "help:"]:
			# This is something we'll be interested in recursing into.
			children.append(child_name)

	children.sort()

	# Descend downwards into the list of childrem.
	for child_name in children:
		# Handle opening/closing of files as we traverse the top levels of the tree
		if len(nodes)==1:
			# Only generate one header for each top-level node
			fname="generated/gsp_tsapi_%s.h" % re.sub('[^-_0-9a-zA-Z]', '', child_name)
			guard_var="GSP_TSAPI_%s_H_INCLUDED" % re.sub('[^_0-9a-zA-Z]', '', re.sub('-', '_', child_name)).upper()
			header_file=open(fname, "a")
			if (not header_files.has_key(fname)):
				header_files[fname] = guard_var
				header_file.write("""/**
 * @file """ + fname + """
 *
 * Generated type-safe API for the """ + child_name + """ branch of the CACPD
 * parameter and management trees.
 *
 * This code is automatically generated.  DO NOT EDIT.
 */

#ifndef """ + guard_var + """
#define """ + guard_var + """

""")
				header_file.write("#include \"type_cacpd_client.h\"\n")
				header_file.write("#include \"type_wrappers.h\"\n")

		if len(nodes)==1 or len(nodes)==2:
			# For the source, we generate more (smaller) files which will
			# link more space-efficiently.
			if len(nodes)==2:
				filename = '_'.join(map(lambda name:c_path_component(name), [nodes[1][0], (child_name=='*' and 'star0' or child_name)]))
			else:
				filename = c_path_component(child_name)
			source_file=open("generated/gsp_tsapi_impl_%s.c" % filename, "a")
			if (source_file.tell() == 0):
				source_file.write("""#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#include "type_wrappers.h"
#include "parameter_types.h"
#include "type_cacpd_client.h"
#include "type_cacpd_client_internal.h"
#include <libcacpdparser/urlcoding.h>

#include <dlog.h>

""")

		# Visit this child...
		nodes.append((child_name, node[child_name]))
		traverse_tree(nodes)
		nodes.pop()

		# Close files if needed.
		if len(nodes)==1 or len(nodes)==2:
			source_file.close()
		if len(nodes)==1:
			
			header_file.close()

# Intialize.
pathgens_already_written=[]
header_files={}

# Walk the trees and produce the API.
traverse_tree([("", parameter_tree.cn_tree)])
traverse_tree([("", management_tree.cn_tree)])

# Close all the "#ifdef" guards:
for fn in header_files.keys():
	header_file = open(fn, "a")
	header_file.write("#endif /* " + header_files[fn] + " */")

