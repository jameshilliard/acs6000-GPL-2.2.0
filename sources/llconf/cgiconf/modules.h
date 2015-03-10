
struct cgiconf_module
{
  struct cgiconf_module *next;
  char *name;
  int (*main)();
};

extern struct cgiconf_module *cgiconf_modules;

int register_cgiconf_module(struct cgiconf_module *cm);
struct cgiconf_module *find_cgiconf_module(const char *name);
