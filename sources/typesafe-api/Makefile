# DEBUG makes the cacpd_client files very noisy
DEBUG = 

## file name prefixs
TSAPI_PRE_PRE = gsp_tsapi
TSAPI_H = ${TSAPI_PRE_PRE}.h
TSAPI_PRE = ${TSAPI_PRE_PRE}_

## Path bits
GSP_PREFIX = ${STAGING_DIR}/${HOST_SYS}
INCLUDE_PATHS = -I${STAGING_INCDIR}


## Flags
STD_CFLAGS =  -fPIC $(shell pkg-config --cflags glib-2.0) ${INCLUDE_PATHS} -Wall -Werror ${DEBUG}
CFLAGS = ${STD_CFLAGS}

LIBS = $(shell pkg-config --libs glib-2.0)  -ldaemon -lcacpdparser
# as does this

OBJS = build/type_cacpd_client.o build/type_wrappers.o

# need to build static library of all gsp_tsapi stuff
TSAPI_LIB_SRC := $(wildcard generated/${TSAPI_PRE}*.c)
# list of all file starting with TSAPI_PRE
TSAPI_LIB_OBJ := $(subst .c,.o, $(subst generated,build, ${TSAPI_LIB_SRC}))
# make a list of object files from the c files
TSAPI_LIB = ${TSAPI_PRE_PRE}

ifeq ($(origin GPL_TAR), undefined)
GPL_TAR=no
endif

ifeq ($(origin GPL_ENV), undefined)
GPL_ENV=no
endif

ifeq ($(GPL_ENV),yes)
TYPES_PATH=generated
else
TYPES_PATH=${STAGING_ROOT}/etc/cacpd
endif

all: ${TSAPI_H} build/lib${TSAPI_LIB}.a install


### TSAPI ######################################################################
# build stuff TSAPI object files into a library. 
build/lib${TSAPI_LIB}.a: ${TSAPI_H} ${OBJS}
	sh -x -c "cd build && for f in ../generated/${TSAPI_PRE}*.c ; do $(CC) -c $(CFLAGS) -I../src/ -I../generated/ \$$f || exit 1 ; done"
	ar rcs build/lib${TSAPI_LIB}.a build/${TSAPI_PRE}*.o ${OBJS}

# build TSAPI objects
build/${TSAPI_PRE}*.o: ${TSAPI_H} generated/type_wrappers.h $(subst build,generated,$(subst .o,.c, $@))
	${CC} -c -o $@ ${CFLAGS} $^

# build the TSAPI sources
${TSAPI_H}: dumped_parameter_tree
	# Generate the source and headers
	PYTHONPATH=./generated/ python generator/mk_typesafe_api.py ${TYPES_PATH}/types.conf $<

	# Make the headers pretty
	#indent generated/gsp_tsapi_*.h

# call to run cacpd with --dump-tree-and-exit
# cacpd needs to be built with --enable-fe-trace
dumped_parameter_tree: generated/management_tree.py generated/parameter_tree.py
	@echo $@: updated

generated/management_tree.py generated/parameter_tree.py:
	@mkdir -p generated/
	@echo 'NOTE: You can ignore any errors saying something "is not a defined function",'
	@echo '      and the following "Parsing tree failed at" error as well.'
	@echo '      These do NOT cause any problem for the typesafe API generation.'
	@echo
	@if ! cacpd --root=${STAGING_ROOT}/ -n --dump-tree-to-path-and-exit=./generated/ ; then \
		echo "CACPD did not generate a tree!" ; \
		exit 1 ; \
	fi
	@echo 'Okay, you can stop ignoring errors now.  Any errors below here are real errors.'

### assistant files ############################################################
src/config.h:
	@(\
		[ -n "$(ENABLE_GSPAAA)" ] && echo "#define ENABLE_GSPAAA 1" || echo "#undef ENABLE_GSPAAA" \
	) > src/config.h

build/type_cacpd_client.o: src/type_cacpd_client.c src/config.h generated/parameter_types.h
	mkdir -p build/
	${CC} -c -o $@ ${CFLAGS} -Igenerated/ src/type_cacpd_client.c
# need to make type_wrappers
build/type_wrappers.o: generated/type_wrappers.c generated/parameter_types.h
	${CC} -c -o $@ ${STD_CFLAGS} -Igenerated/ generated/type_wrappers.c

generated/type_wrappers.c generated/type_wrappers.h generated/parameter_types.h: tools/mk_wrappers ${TYPES_PATH}/types.conf
	tools/mk_wrappers ${TYPES_PATH}/types.conf

tools/mk_wrappers: generator/mk_wrappers.c
	@mkdir -p tools/
	${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS} -Wall -Werror -lllconf -o $@ $<

install:
ifeq ($(GPL_TAR),yes)
	install -d $(DL_DIR)/typesafe-api/generated
	install generated/*.py $(DL_DIR)/typesafe-api/generated
	install ${STAGING_ROOT}/etc/cacpd/types.conf $(DL_DIR)/typesafe-api/generated
	install ${STAGING_INCDIR}/parameter-ctypes.h $(DL_DIR)/typesafe-api/generated
endif
ifeq ($(GPL_ENV),yes)
	install generated/parameter-ctypes.h ${STAGING_INCDIR}/
endif

# PHONY targets
docs:
	doxygen
clean_tsapi:
	rm -rf $(wildcard ${TSAPI_PRE}*.[ch]) ${TSAPI_H} lib${TSAPI_LIB}.a
clean:
	rm -rf doc generated/*.c generated/*.h build/*.o build/*.a
