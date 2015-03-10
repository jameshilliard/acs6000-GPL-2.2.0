/**
 * @file mk_wrappers.c
 *
 * Generates the C headers and source code for "type wrappers", which are
 * responsible for converting between CACPD data types and native C data types.
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <llconf/nodes.h>
#include <llconf/entry.h>
#include <llconf/modules.h>
#include <llconf/allmodules.h>
#include <llconf/lines.h>

#define DLOG_CONTEXT "TSAPI"

/**
 * The macro name to use when guarding against multiple inclusion.
 */
#define GUARD_MACRO "GSP_TSAPI__TYPE_WRAPPERS_H_INCLUDED"
#define GUARD_MACRO_TYPES "GSP_TSAPI__PARAMETER_TYPES_H_INCLUDED"

static int exit_code = 0;

/**
 * A list of include file names, each with a flag indicating whether it's required for the interface.
 */
struct includefile {
	struct includefile *next; /**< The next includefile in the list, or NULL if the end of the list. */
	char *name; /**< The name of the include file, including the quotes or angle-brackets. */
	int needed_for_interface; /**< Nonzero if the file is required for the type_wrappers interface. */
};

/**
 * A list of parameter type definitions.
 */
struct paramtype {
	struct paramtype *next; /**< The next paramtype in the list, or NULL if the end of the list. */
	struct paramtype *parent_type; /**< The parent_type transitively provides definitions which are absent from this definition. */
	char *name; /**< The name of the parameter, as seen by CACPD. */
	char *c_type; /**< The name of the native C type to which this parameter will be mapped. */
	char *define; /**< If set, this type will define an enum with the given FULL name, i.e. "enum foo" */
	char *from_c; /**< Text C source code to convert from c_type to paramtype. */
	char *to_c; /**< Text C source code to convert from paramtype to c_type. */
	char *to_c_free; /**< Text C source code to free any memory allocated by to_c. */
	struct cnfnode *enum_list; /**< The data structure containing the information about the enumeration of this type. */
};

/**
 * The big list of type structures.
 */
struct paramtype *param_type_list = NULL;

/**
 * The include files requested by the types.
 */
struct includefile *includes = NULL;

/**
 * The data to write into types.conf for CACPD to use at runtime.
 */
struct cnfnode *runtime_types_conf = NULL;



/**
 * Add a single include-file to the global list of includes.
 * @param [in] include_name The full name of the include file, including quotes
 *	or angle-brackets.
 * @param [in] needed_for_interface Nonzero if this include file should be in
 *	the type_wrappers header.
 */
static
void add_include(const char *include_name, int needed_for_interface)
{
	struct includefile *i;
	for (i = includes; i; i = i->next)
		if (0 == strcmp(i->name, include_name)){
			if (needed_for_interface)
				i->needed_for_interface = 1;
			return;
		}

	/* Prepend to list */
	i = malloc(sizeof(struct includefile));
	i->needed_for_interface = needed_for_interface;
	i->name = strdup(include_name);

	i->next = includes;
	includes = i;
}

/**
 * Add a space-separated list of include files to the global list of includes.
 * Calls #add_include for each element of the given list.
 * @param [in] include_names The space-delimited list of include file names,
 *	including quotes or angle-brackets around each element as appropriate.
 * @param [in] needed_for_interface Nonzero if this include file should be in
 *	the type_wrappers header.
 */
static
void add_includes(const char *include_names, int needed_for_interface)
{
	char *working_ptr = strdup(include_names);
	char *working_copy = working_ptr;
	char *token;
	char *saveptr;
	while ((token = strtok_r(working_copy, " \t\n", &saveptr))) {
		working_copy = NULL;
		add_include(token, needed_for_interface);
	}
	free(working_ptr);
}

/**
 * Dump the list of includes.
 * @param [in] outfile The file into which the list is written.
 * @param [in] i The head of the list to dump.
 * @param [in] for_interface Nonzero if we're dumping the header.
 */
static
void dump_includes(FILE *outfile, struct includefile *i, int for_interface)
{
	/* Recurse so the list comes out forwards ;-) */
	if (i->next)
		dump_includes(outfile, i->next, for_interface);
	
	if (for_interface == i->needed_for_interface)
		fprintf(outfile, "#include %s\n", i->name);
}

/**
 * Finds the paramtype for a given type name.
 * @param [in] typename The name of the type to seek.
 * @return The #paramtype structure, or NULL if not found.
 */
static
struct paramtype *find_type(const char *typename)
{
	struct paramtype *t;
	for(t = param_type_list; t; t = t->next) {
		if (0 == strcmp(t->name, typename))
			break;
	}
	return t;
}

/**
 * Parses the types.conf definition of a single type, returning the parsed
 * information in a #paramtype structure.
 * @param [in] cn_top The node representing the source data for the type.
 * @return The parsed #paramtype structure.
 */
static
struct paramtype *parse_type(struct cnfnode *cn_top)
{
	struct paramtype *n = malloc(sizeof(struct paramtype));
	memset(n, 0, sizeof(struct paramtype));
	n->name = strdup(cn_top->name);
	
	struct cnfnode *runtime_type = create_cnfnode(cn_top->name);
	append_node(runtime_types_conf, runtime_type);

	struct cnfnode *child = cn_top->first_child;
	for (child = cn_top->first_child; child; child = child->next) {
		if (0 == strcmp("fromC:", child->name))
			n->from_c = strdup(child->value);
		else if (0 == strcmp("toC:", child->name))
			n->to_c = strdup(child->value);
		else if (0 == strcmp("toC-free:", child->name))
			n->to_c_free = strdup(child->value);
		else if (0 == strcmp("parent-type:", child->name)) {
			n->parent_type = find_type(child->value);
			append_node(runtime_type, clone_cnfnode(child));
		} else if (0 == strcmp("ctype:", child->name)) {
			n->c_type = strdup(child->value);
/*			if (MALLOC_UNKNOWN == n->malloc)
				n->malloc = MALLOC_NO; / * If a new ctype is given, assume it's not malloced unless we're told otherwise. */
		} else if (0 == strcmp("fromC-format:", child->name)) {
			if (
				asprintf(&n->from_c,
					"\tchar *string_value=NULL;\n\n\tif (asprintf(&string_value, %s, c_value) < 0)\n\t\tstring_value = NULL;\n\n\treturn string_value;",
					child->value) < 0
			) {
				exit_code = 11;
				return n;
			}
		} else if (0 == strcmp("flags:", child->name)) {
			struct cnfnode *flags_node = create_cnfnode(child->name);
			cnfnode_setval(flags_node, (
				strcasestr(child->value, "case-insensitive") ?
				"case-insensitive" : ""
			));
			append_node(runtime_type, flags_node);
		} else if (0 == strcmp("define:", child->name)) {
			n->define = strdup(child->value);
			n->c_type = strdup(child->value);
		} else if (0 == strcmp("enum", child->name)) {
			n->enum_list = clone_cnftree(child);
		} else if ((0 == strcmp("includes:", child->name)) ||
			(0 == strcmp("include:", child->name))) {
			add_includes(child->value, 1);
		} else if ((0 == strcmp("implementation-includes:", child->name)) ||
			(0 == strcmp("implementation-include:", child->name))) {
			add_includes(child->value, 0);
		} else if ((0 == strcmp("validation:", child->name)) ||
			(0 == strcmp("len:", child->name)) ||
			(0 == strcmp("choices:", child->name))) {
			append_node(runtime_type, clone_cnfnode(child));
		} else
		{
			fprintf(stderr, "Unable to parse element '%s' in type '%s' in 'types.conf'.\n", child->name, n->name);
			exit_code = 10;
		}
	}

	return n;
}

/**
 * Parses the types.conf file.
 * @param [in] src The node representing the source data.
 * @return The head of a list of parsed #paramtype structures.
 */
static
struct paramtype *parse_type_list(struct cnfnode *src)
{
	struct cnfnode *s;
	struct paramtype *head = NULL, *tail = NULL;
	for (s = src->first_child; s; s = s->next) {
		struct paramtype *new_type = parse_type(s);

		/* Append to list */
		if (tail) {
			tail->next = new_type;
		} else {
			head = new_type;
			param_type_list = new_type;
		}
		tail = new_type;
	}
	return head;
}


/**
 * Generate the doxygen comments for the from_c function.
 * @param [in] paramtype The #paramtype for which the documentation should be
 *	generated.
 * @return The text of the doxygen comments.
 */
static
char *from_c_documentation(struct paramtype *p)
{
	char *result = NULL;
	if (0 > asprintf(&result, "/**\n * Converts a C '%s' into a serialized value of type '%s'\n"
                        " * that the GSP core will understand. The GSP core accepts all data as UTF8\n"
                        " * strings; this function serializes the native C '%s' value into that format.\n"
                        " * @param [in] c_value The native C source data.\n"
                        " * @return A pointer to a malloc()ed string with the serialized data.  The caller must\n"
                        " *         free this pointer when done.\n"
                        " */", p->c_type, p->name, p->c_type))
		result = NULL;
	return result;
}

/**
 * Generate the doxygen comments for the to_c function.
 * @param [in] paramtype The #paramtype for which the documentation should be
 *	generated.
 * @return The text of the doxygen comments.
 */
static
char *to_c_documentation(struct paramtype *p)
{
	char *result = NULL;
	if (0 > asprintf(&result,
                        "/**\n * Converts a UTF8 serialized representation of a GSP core '%s' value\n"
                        " * into a native C '%s'.\n"
                        " * The GSP core accepts all data as UTF8 strings; this function unserializes the GSP core\n"
                        " * response into the native C '%s' type.\n"
                        " * @param [in] string_value The serialized UTF8 source data.\n"
			" * @param [out] c_value A pointer to the storage for the native C type.\n"
//                        " * @return A %s%s with the native data.\n%s"
			"%s"
			" * @return Nonzero for a successful conversion; zero on error.\n"
                        " */",
			p->name, p->c_type, p->c_type,
//			(p->to_c_free ? "malloc()ed " : ""), p->c_type,
			(p->to_c_free ? " * The caller must free this pointer when done.\n" : "")))
		result = NULL;
	return result;
}

/**
 * Generate the doxygen comments for the to_c_free function.
 * @param [in] paramtype The #paramtype for which the documentation should be
 *	generated.
 * @return The text of the doxygen comments.
 */
static
char *to_c_free_documentation(struct paramtype *p)
{
	char *result = NULL;
	if (0 > asprintf(&result,
                        "/**\n * Frees the memory used by a native representation of a GSP core '%s' value.\n"
                        " * @param [in] c_value The item to free.\n"
                        " */",
			p->name
		))
		result = NULL;
	return result;
}

/**
 * Generate the doxygen comments for the to_c_free function.
 * @param [in] paramtype The #paramtype for which the documentation should be
 *	generated.
 * @return The text of the doxygen comments.
 */
static
char *to_c_list_documentation(struct paramtype *p)
{
	char *result = NULL;
	if (0 > asprintf(&result,
                        "/**\n * Converts a list of UTF8 serialized representations of GSP core '%s' values\n"
			" * into a list of native C '%s'.\n"
			" * @param [in] src_list_head The first item in the source list.\n"
			" * @param [out] new_list_head Storage into which a pointer to the new list is placed.\n"
			" * @return Zero on success, nonzero on error.\n"
                        " */",
			p->name, p->c_type
		))
		result = NULL;
	return result;
}

/**
 * Generate the doxygen comments for the to_c_free function.
 * @param [in] paramtype The #paramtype for which the documentation should be
 *	generated.
 * @return The text of the doxygen comments.
 */
static
char *list_free_documentation(struct paramtype *p)
{
	char *result = NULL;
	if (0 > asprintf(&result,
                        "/**\n * Frees the memory used by a list of native representations of GSP core '%s' values.\n"
                        " * @param [in] list_head The first item in the list to free.\n"
                        " */",
			p->name
		))
		result = NULL;
	return result;
}


/**
 * Generate the function prototype for the from_c function.
 * @param [in] paramtype The #paramtype for which the prototype should be
 *	generated.
 * @param [in] is_private If nonzero, prefix the function name with an
 *	underscore, so we can make a private version that includes logging.
 * @return The text of the function prototype.
 */
static
char *from_c_prototype(struct paramtype *p, int is_private)
{
	char *result = NULL;
	if (asprintf(&result, "%schar *%sparamtype_%s_fromC(const %s c_value)",
			(is_private ? "static " : ""), (is_private ? "_" : ""),
			p->name, p->c_type
		) < 0)
		result = NULL;

	return result;
}

/**
 * Generate the function prototype for the to_c function.
 * @param [in] paramtype The #paramtype for which the prototype should be
 *	generated.
 * @param [in] is_private If nonzero, prefix the function name with an
 *	underscore, so we can make a private version that includes logging.
 * @return The text of the function prototype.
 */
static
char *to_c_prototype(struct paramtype *p, int is_private)
{
	char *result = NULL;
	if (asprintf(&result, "%sint %sparamtype_%s_toC(const char *string_value, %s *c_value)",
			(is_private ? "static " : ""), (is_private ? "_" : ""),
			p->name, p->c_type
		) < 0)
		result = NULL;

	return result;
}

/**
 * Generate the function prototype for the to_c_free function.
 * @param [in] paramtype The #paramtype for which the prototype should be
 *	generated.
 * @return The text of the function prototype.
 */
static
char *to_c_free_prototype(struct paramtype *p)
{
	char *result = NULL;
	if (asprintf(&result, "void paramtype_%s_toC_free(%s c_value)",
			p->name,
			p->c_type
		) < 0)
		result = NULL;

	return result;
}

/**
 * Generate the function prototype for the to_c_list function.
 * @param [in] paramtype The #paramtype for which the prototype should be
 *	generated.
 * @return The text of the function prototype.
 */
static
char *to_c_list_prototype(struct paramtype *p)
{
	char *result = NULL;
	if (asprintf(&result, "int paramtype_%s_list_toC(struct paramtype_UTF8String_list *src_list_head,\n\t\tstruct paramtype_%s_list **new_list_head)",
		p->name, p->name
	) < 0)
		result = NULL;

	return result;
}

/**
 * Generate the function prototype for the to_c_list_free function.
 * @param [in] paramtype The #paramtype for which the prototype should be
 *	generated.
 * @return The text of the function prototype.
 */
static
char *list_free_prototype(struct paramtype *p)
{
	char *result = NULL;
	if (asprintf(&result, "void paramtype_%s_list_free(struct paramtype_%s_list *list_head)",
		p->name, p->name
	) < 0)
		result = NULL;

	return result;
}

static
char *to_c_list_code(struct paramtype *p)
{
	char *result = NULL;
	if (asprintf(&result, "\tstruct paramtype_UTF8String_list *cur;\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"Converting a list of %s parameters to %s\");\n"
		"\tint i;\n"
		"\t*new_list_head = NULL;\n"
		"\tstruct paramtype_%s_list *new_list_tail = NULL;\n"
		"\tfor(cur = src_list_head, i = 0; cur; cur = cur->next, i++) {\n"
		"\t\tstruct paramtype_%s_list *n = (struct paramtype_%s_list *)calloc(1, sizeof(struct paramtype_%s_list));\n"
		"\t\tif (!paramtype_%s_toC(cur->data, &n->data)) {\n"
		"\t\t\tfree(n);\n"
		"\t\t\tparamtype_%s_list_free(*new_list_head);\n"
		"\t\t\t*new_list_head = NULL;\n"
		"\t\t\tdlog_printf(DLOG_ERROR, \""DLOG_CONTEXT"\", \"Failed to convert list due to unconvertible element \\\"%%s\\\" at index %%i.\", cur->data, i);\n"
		"\t\t\treturn 1;\n"
		"\t\t}\n"
		"\t\tif(!*new_list_head) {\n"
		"\t\t\t*new_list_head = new_list_tail = n;\n"
		"\t\t} else {\n"
		"\t\t\tnew_list_tail->next = n;\n"
		"\t\t\tnew_list_tail = n;\n"
		"\t\t}\n"
		"\t}\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"Converted %%i list elements.\", i);\n"
		"\treturn 0;", p->name, p->c_type, p->name, p->name, p->name, p->name, p->name, p->name) < 0)
		result = NULL;

	return result;
}

static
char *list_free_code(struct paramtype *p)
{
	char *result = NULL;
	char *free_func = NULL;
	if (p->to_c_free)
		if (asprintf(&free_func, "\t\tparamtype_%s_toC_free(list_head->data);\n", p->name) < 0)
			return NULL;

	if (asprintf(&result, "\tstruct paramtype_%s_list *next = list_head;\n"
		"\twhile(list_head) {\n"
		"\t\tnext = list_head->next;\n"
		"%s"
		"\t\tfree(list_head);\n"
		"\t\tlist_head = next;\n"
		"\t}", p->name, (free_func ? free_func : "")) < 0)
		result = NULL;
	if (free_func)
		free(free_func);

	return result;
}

/**
 * Generate the C header (".h") content for defining the native C equivalent
 * of a parameter type, if needed.
 * @param [in] outfile The file to which the content should be written.
 * @param [in] p The parameter for which the header information should be
 *	written.
 */
static
void generate_definitions(FILE *outfile, struct paramtype *p)
{
	if (p->define && p->enum_list) {
		fprintf(outfile, "\n\n/**\n"
			" * Enumeration for parameter type '%s':\n"
			" */\n"
			"%s {\n", p->name, p->define);
		struct cnfnode *e;
		for (e = p->enum_list->first_child; e; e = e->next)
			fprintf(outfile, "\t%s, /**< Corresponds to parameter value '%s'. */\n", cnf_get_entry(e, "cValue:"), e->name);
		fprintf(outfile, "};\n\n");
		fprintf(outfile, "/**\n * Parameter type 'paramtype_%s' is a C '%s'.\n */\n",
			p->name, p->define);
		fprintf(outfile, "typedef %s paramtype_%s;", p->define, p->name);
	} else {
		fprintf(outfile, "/**\n * Parameter type 'paramtype_%s' is a C '%s'.\n */\n",
			p->name, p->c_type);
		fprintf(outfile, "typedef %s paramtype_%s;", p->c_type, p->name);
	}

	fprintf(outfile, "\n\n/**\n"
		" * List type for parameter type '%s':\n"
		" */\n"
		"struct paramtype_%s_list {\n"
		"	struct paramtype_%s_list *next; /**< The next node in the list.  NULL indicates the end of the list. */\n"
		"	%s data; /**< The type-safe data at this node. */\n"
		"};\n",
		p->name, p->name, p->name, p->c_type);
}

/**
 * Generate the C header (".h") content for the conversion functions for a
 * single parameter type, writing it to a file.
 * @param [in] outfile The file to which the content should be written.
 * @param [in] p The parameter for which the header information should be
 *	written.
 */
static
void generate_header(FILE *outfile, struct paramtype *p)
{
	fprintf(outfile,
		"\n\n/*\n"
		" * Functions for converting parameter type '%s' <==> C type '%s'.\n"
		" */\n",
		p->name, p->c_type
	);
	
	char *doc = from_c_documentation(p);
	char *proto = from_c_prototype(p, 0);
	fprintf(outfile, "%s\n%s;\n", doc, proto);
	free(doc);
	free(proto);

	doc = to_c_documentation(p);
	proto = to_c_prototype(p, 0);
	fprintf(outfile, "\n%s\n%s;\n", doc, proto);
	free(doc);
	free(proto);

	if (p->to_c_free) {
		doc = to_c_free_documentation(p);
		proto = to_c_free_prototype(p);
		fprintf(outfile, "\n%s\n%s;\n", doc, proto);
		free(doc);
		free(proto);
	}

	/* Generate code for converting UTF8String to other types. */
	if (strcmp("UTF8String", p->name)){
		doc = to_c_list_documentation(p);
		proto = to_c_list_prototype(p);
		fprintf(outfile, "\n%s\n%s;\n", doc, proto);
		free(doc);
		free(proto);
	}

	doc = list_free_documentation(p);
	proto = list_free_prototype(p);
	fprintf(outfile, "\n%s\n%s;\n", doc, proto);
	free(doc);
	free(proto);
}

/**
 * Generate the C source (".c") content for a single parameter type, writing it
 * to a file.
 * @param [in] outfile The file to which the content should be written.
 * @param [in] p The parameter for which the C source code should be
 *	written.
 */
static
void generate_source(FILE *outfile, struct paramtype *p)
{
	fprintf(outfile,
		"\n\n/*\n"
		" * Functions for converting parameter type '%s' <==> C type '%s'.\n"
		" */\n\n",
		p->name, p->c_type
	);
	
	char *proto = from_c_prototype(p, 1); /* The immediate type wrapper. */
	fprintf(outfile, "\n%s\n{\n%s\n}\n", proto, p->from_c);
	free(proto);

	char *doc = from_c_documentation(p);
	proto = from_c_prototype(p, 0); /* The public function */
	fprintf(outfile, "\n%s\n%s\n"
		"{\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"\\\"fromC:\\\" function for parameter type %s will be asked to convert a value of type (%s) to a string.\");\n"
		"\tchar *string_value = _paramtype_%s_fromC(c_value);\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"   successfully converted the value of type (%s) to the string \\\"%%s\\\".\", string_value);\n"
		"\treturn string_value;\n"
		"}\n",
		doc, proto, p->name, p->c_type, p->name, p->c_type
	);
	free(proto);
	free(doc);

	proto = to_c_prototype(p, 1);
	fprintf(outfile, "\n%s\n{\n%s\n}\n", proto, p->to_c);
	free(proto);

	doc = to_c_documentation(p);
	proto = to_c_prototype(p, 0); /* The public function */
	fprintf(outfile, "\n%s\n%s\n"
		"{\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"\\\"toC:\\\" function for parameter type %s will be asked to convert the string \\\"%%s\\\" to a value of type (%s).\", string_value);\n"
		"\tint result = _paramtype_%s_toC(string_value, c_value);\n"
		"\tdlog_printf(DLOG_DEBUG, \""DLOG_CONTEXT"\", \"   %%ssuccessfully converted the string \\\"%%s\\\" to type (%s).\", (result ? \"\" : \"un\"), string_value);\n"
		"\treturn result;\n"
		"}\n",
		doc, proto, p->name, p->c_type, p->name, p->c_type
	);
	free(doc);
	free(proto);

	if (p->to_c_free) {
		doc = to_c_free_documentation(p);
		proto = to_c_free_prototype(p);
		fprintf(outfile, "\n%s\n%s\n{\n%s\n}\n", doc, proto, p->to_c_free);
		free(doc);
		free(proto);
	}

	/* Generate code for converting UTF8String to other types. */
	if (strcmp("UTF8String", p->name)){
		doc = to_c_list_documentation(p);
		proto = to_c_list_prototype(p);
		char *code = to_c_list_code(p);
		fprintf(outfile, "\n%s\n%s\n{\n%s\n}\n", doc, proto, code);
		free(code);
		free(doc);
		free(proto);
	}

	doc = list_free_documentation(p);
	proto = list_free_prototype(p);
	char *code = list_free_code(p);
	fprintf(outfile, "\n%s\n%s\n{\n%s\n}\n", doc, proto, code);
	free(doc);
	free(proto);
}


/**
 * A callback for generating the code for handling a case of an enumerated
 * type.
 */
typedef char *(*enum_code_generator)(struct cnfnode *);


/**
 * Generate the C source for handling a single case of an enumerated type in a
 * from_c function.  The generated code compares the native enum value against
 * the value of "this entry" of the enumerated type, and if there's a match,
 * returns the *name* of this value of the enum (i.e. the name by which CACPD
 * knows this value)..
 * @param [in] enum_entry The enum value for which the code is to be generated.
 * @return Text C source code for handling the given enumerated value.
 */
static
char *enum_from_c_element(struct cnfnode *enum_entry)
{
	char *result = NULL;
	if (0 > asprintf(&result, "if (%s == c_value) return strdup(\"%s\");\n", cnf_get_entry(enum_entry, "cValue:"), enum_entry->name))
		result = NULL;

	return result;
}

/**
 * Generate the C source for handling a single case of an enumerated type in a
 * to_c function.  The generated code performs a case-sensitive comparison
 * between the input value (string_value) and the name of "this entry" of the
 * enumerated type (as known to CACPD).  If there's a match, it returns the
 * corresponding native enum value.
 * @param [in] enum_entry The enum value for which the code is to be generated.
 * @return Text C source code for handling the given enumerated value.
 */
static
char *enum_to_c_element(struct cnfnode *enum_entry)
{
	char *result = NULL;
	if (0 > asprintf(&result, "if (0 == strcmp(\"%s\", string_value)) {*c_value = %s; return 1;}\n", enum_entry->name, cnf_get_entry(enum_entry, "cValue:")))
		result = NULL;

	return result;
}

/**
 * Generate and return the C source code for handling an entire enumerated type.
 * @param [in] enum_top The enum type for which the code is to be generated.
 * @param [in] element_generator The function which will generate a line of
 *	source for each possible value of the enumeration.
 * @param [in] default_code Code to execute if none of the enumerations match.
 * @return Text C source code for handling the given enumerated type.
 */
static
char *generate_enum_code(struct cnfnode *enum_top, enum_code_generator element_generator, const char *default_code)
{
	struct confline *cl = NULL;
	struct cnfnode *enum_entry;
	for (enum_entry = enum_top->first_child; enum_entry; enum_entry = enum_entry->next) {
		char *line = element_generator(enum_entry);
		cl = append_confline(cl, create_confline(line));
		free(line);
	}
	cl = append_confline(cl, create_confline(default_code));

	/* Stick it all together... */
	struct confline *c;
	int len = 0;
	for(c = cl; c; c = c->next) {
		len += strlen(c->line);
	}

	char *result = malloc(len + 1);
	result[0] = '\0';
	char *p = result;
	for(c = cl; c; c = c->next) {
		int l = strlen(c->line);
		strncat(p, c->line, len);
		p += l;
		len -= l;
	}
	destroy_confline_list(cl);
	return result;
}

/**
 * Generate and return the C source code for converting an enumerated type
 * from a native C type to its corresponding "parameter type" (i.e. as seen
 * by CACPD).
 * @param [in] p The parameter type for which the code should be generated.
 */
static
char *generate_enum_from_c(struct paramtype *p)
{
	return generate_enum_code(p->enum_list, enum_from_c_element, "return strdup(\"\");");
}

/**
 * Generate and return the C source code for converting an enumerated type
 * from a CACPD parameter type to its corresponding native C type.
 * @param [in] p The parameter type for which the code should be generated.
 */
static
char *generate_enum_to_c(struct paramtype *p)
{
	return generate_enum_code(p->enum_list, enum_to_c_element, "return 0;");
}


/**
 * Traverse "parent_type" links to determine the c_type which should apply to
 * the given parameter type.  Note that this type, or its parents, may use
 * "define" to define a new type, which automatically becomes the c_type as
 * well.
 * @param [in] p The parameter for which the c_type is required.
 * @return A malloced string containing the c_type which should be used, or the
 *	empty string if none could be determined.
 */
static
char *find_c_type_for(struct paramtype *p)
{
	if (p->c_type)
		return strdup(p->c_type);
	if (p->define)
		return strdup(p->define);
	if (p->parent_type)
		return find_c_type_for(p->parent_type);
	return strdup("");
}

/**
 * Fills in any missing information (c_type, malloc, from_c, to_c) using parent
 * types and enum definitions.  When this function returns, all #paramtype
 * structures should have c_type, malloc, from_c and to_c elements defined
 * where suitable values can be determined.
 * @return Nonzero on success (i.e. everything resolved nicely).  Zero on
 *	failure (i.e. something wasn't defined).
 */
static
int fill_in_the_blanks()
{
	struct paramtype *p;
	int ok = 1;
	for (p = param_type_list; p; p = p->next) {
		/* Find the c_type, if it's not yet known. */
		if (!p->c_type)
			p->c_type = find_c_type_for(p);

		if (p->enum_list && !p->to_c)
			p->to_c = generate_enum_to_c(p);

		if (p->enum_list && !p->from_c)
			p->from_c = generate_enum_from_c(p);

		if (p->parent_type && !p->to_c) {
			if (0 > asprintf(&p->to_c, "return paramtype_%s_toC(string_value, c_value);",
					p->parent_type->name))
				p->to_c = NULL;
		}
		if (p->parent_type && !p->from_c) {
			if (0 > asprintf(&p->from_c, "return paramtype_%s_fromC(c_value);",
					p->parent_type->name))
				p->from_c = NULL;
		}

		/*
		 * Check that everything worked
		 */
		if (!p->from_c) {
			fprintf(stderr, "ERROR: Unable to find or generate a 'from_c' function for type '%s'.\n", p->name);
			ok = 0;
		}
		if (!p->to_c) {
			fprintf(stderr, "ERROR: Unable to find or generate a 'to_c' function for type '%s'.\n", p->name);
			ok = 0;
		}
		if (!p->c_type) {
			fprintf(stderr, "ERROR: Unable to determine a c_type for type '%s'.\n", p->name);
			ok = 0;
		}
	}
	return ok;
}

/**
 * Write a standard file header to an output file.
 * @param [in] outfile The file to which the file header should be written.
 * @param [in] filename The name of the file we're writing.
 */
static
void generate_file_header(FILE *outfile, const char *filename)
{
	fprintf(outfile,
		"/**\n"
		" * @file %s\n"
		" *\n"
		" * Type-wrappers for the GSP Core type-safe API.\n"
		" *\n"
		" * A set of \"toC\" and \"fromC\" wrappers are generated for each GSP Core type.\n"
		" * These wrappers are in turn used by the type-safe API generator to construct\n"
		" * a type-safe API for access to the entire configuration tree.\n"
		" */\n"
		"\n",
		filename);
}


/**
 * Generate the entire C header file (".h"), defining or including the types
 * from the previously-parsed list of parameter types.
 * @param [in] outfile The file to which the C header content should be
 *	written.
 */
static
void generate_parameter_types_header_file(FILE *outfile)
{
	struct paramtype *p;
	generate_file_header(outfile, "parameter_types.h");
	fprintf(outfile, "#ifndef "GUARD_MACRO_TYPES"\n"
		"#define "GUARD_MACRO_TYPES"\n\n");
	dump_includes(outfile, includes, 1);
	for (p = param_type_list; p; p = p->next) {
		generate_definitions(outfile, p);
	}
	fprintf(outfile, "\n#endif /* "GUARD_MACRO_TYPES" */\n");
}

/**
 * Generate the entire C header file (".h") for the type wrapper functions
 * from the previously-parsed list of parameter types.
 * @param [in] outfile The file to which the C header content should be
 *	written.
 */
static
void generate_header_file(FILE *outfile)
{
	struct paramtype *p;
	generate_file_header(outfile, "type_wrappers.h");
	fprintf(outfile, "#ifndef "GUARD_MACRO"\n"
		"#define "GUARD_MACRO"\n\n"
		"#include \"parameter_types.h\"\n"
		"#include <glib-2.0/glib.h>\n\n");

	/* CACPD-list-to-UTF8String-list */
	fprintf(outfile, "/**\n * Converts a GSList from libcacpdparser into a paramtype_UTF8String_list.\n"
	" * @param [in] src The source data list, which must be a GSList * with each node containing a char *.\n"
	" *		This list will have been freed when this function returns, although the list components\n"
	" *		are *not* freed.  This is as expected by libcacpdparser's function cacpdclient_response_get_list().\n"
	" * @return The head of a new paramtype_UTF8String_list with the same data as the input list.\n"
	" */\n");
	fprintf(outfile, "struct paramtype_UTF8String_list *cacpdclient_list_to_UTF8String_list(GSList *src);\n\n");
	for (p = param_type_list; p; p = p->next) {
		generate_header(outfile, p);
	}
	fprintf(outfile, "\n#endif /* "GUARD_MACRO" */\n");
}

/**
 * Generate the entire C source file (".c") for the type wrappers from the
 * previously-parsed list of parameter types.
 * @param [in] outfile The file to which the C source content should be
 *	written.
 */
static
void generate_source_file(FILE *outfile)
{
	struct paramtype *p;
	generate_file_header(outfile, "type_wrappers.c");
	fprintf(outfile, "#define _GNU_SOURCE\n\n");
	dump_includes(outfile, includes, 0);

	/* CACPD-list-to-UTF8String-list */
	fprintf(outfile,
"struct paramtype_UTF8String_list *cacpdclient_list_to_UTF8String_list(GSList *src)\n"
"{\n"
"	struct paramtype_UTF8String_list *head = NULL, *tail = NULL;\n"
"	GSList *p;\n"
"	for(p = src; p; p = p->next){\n"
"		struct paramtype_UTF8String_list *n = malloc(sizeof(struct paramtype_UTF8String_list));\n"
"		n->next = NULL;\n"
"		n->data = strdup(p->data);\n"
"		if (head){\n"
"			tail->next = n;\n"
"			tail = n;\n"
"		} else {\n"
"			head = tail = n;\n"
"		}\n"
"	}\n\n"
"	g_slist_free(src);\n"
"\n"
"	return head;\n"
"}\n\n");
	for (p = param_type_list; p; p = p->next) {
		generate_source(outfile, p);
	}	
}

/**
 * Generate the "type wrapper" header and source from a given "types.conf".
 * @param [in] argc The number of arguments on the command line.  This should
 *	be exactly two.
 * @param [in] argv The arguments on the command line.  Besides the executable
 *	name in argv[0],
 *	* argv[1] should be the "types.conf" to use.
 * @return A shell return code -- zero on success.
 */
int main(int argc, char **argv)
{
	struct cnfmodule *cyconf;
	if (argc != 2) {
		fprintf(stderr, "Expected usage: %s <in-types.conf>\n", program_invocation_short_name);
		return 12;
	}
	register_all(NULL);

	add_include("\"type_wrappers.h\"", 0);
	add_include("<stdio.h>", 0);
	add_include("<stdlib.h>", 0);
	add_include("<dlog.h>", 0);

	cyconf = find_cnfmodule("cyconf");
	if (!cyconf) {
		fprintf(stderr, "ERROR: Unable to find the 'cyconf' parser.\n");
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "ERROR: Unable to open types.conf.\n");
		return 2;
	}

	struct cnfnode *types = cyconf->parser(cyconf, f);
	fclose(f);

	if (!types) {
		fprintf(stderr, "ERROR: Unable to parse types.conf.\n");
		return 3;
	}
	strip_cnftree(types);

	runtime_types_conf = create_cnfnode("(root)");
	param_type_list = parse_type_list(types);
	destroy_cnftree(types);

	if (!fill_in_the_blanks()) {
		fprintf(stderr, "ERROR: Aborting %s due to error(s).\n",
			program_invocation_short_name);
		return 13;
	}

	FILE *types_header_file = fopen("generated/parameter_types.h", "w");
	generate_parameter_types_header_file(types_header_file);
	fclose(types_header_file);

	FILE *header_file = fopen("generated/type_wrappers.h", "w");
	generate_header_file(header_file);
	fclose(header_file);

	FILE *source_file = fopen("generated/type_wrappers.c", "w");
	generate_source_file(source_file);
	fclose(source_file);

	FILE *runtime_types_conf_file = fopen("generated/runtime-types.conf", "w");
	cyconf->unparser(cyconf, runtime_types_conf_file, runtime_types_conf);
	fclose(runtime_types_conf_file);

	return exit_code;
}
