#ifndef PLUGIN_H
#define PLUGIN_H
/* plugin system interface with sandbox hooks */

int plugin_install(const char *url);
int plugin_load(const char *file);
int plugin_unload(const char *name);
int plugin_exec(const char *name);
void plugin_list(void);
int plugin_register_hook(int (*hook)(const char *));

#endif
