#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>

#include <cgi.h>
#include <session.h>

#include "modules.h"
#include "ifconf.h"

//#include <llconf2/utils.h>

int main(void)
{
  struct cgiconf_module *module;
  char *module_name;

  register_ifconf();

  cgi_init();
  cgi_session_start();
  cgi_process_form();
  cgi_init_headers();
  
  cgi_include("/var/www/cgiconf/html/top.inc");

  module_name = cgi_param("module");

  if(module_name){
    module = find_cgiconf_module(module_name);
    
    if(module){
      module->main();
    }else{
      printf("<p>module %s not found.</p>\n", module_name);
    }
  }else
    printf("<p>hello</p>\n");

  cgi_include("/var/www/cgiconf/html/bottom.inc");

  cgi_end();

  exit(0);
}
