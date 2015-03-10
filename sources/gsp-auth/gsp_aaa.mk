
%.so:
	$(CXX) -shared -Wl,-soname,$(LIB_NAME).so.$(LIB_MAJOR) \
		-o $(@).$(LIB_MAJOR).$(LIB_MINOR) $^ $(LDFLAGS)

%.a:
	$(AR) -rc $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) -fPIC -c $(CFLAGS) $< -o $@

%.o: %.cc
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $< \
		| sed "s/\($*\)\.o[ :]*/\1.o $@ : /g" > $@; \
		[ -s $@ ] || rm -f $@'

%.d: %.cc
	$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $< \
		| sed "s/\($*\)\.o[ :]*/\1.o $@ : /g" > $@; \
		[ -s $@ ] || rm -f $@'


%.d: %.cpp
	$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $< \
		| sed "s/\($*\)\.o[ :]*/\1.o $@ : /g" > $@; \
		[ -s $@ ] || rm -f $@'
