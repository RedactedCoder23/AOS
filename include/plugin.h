#ifndef PLUGIN_H
#define PLUGIN_H

int plugin_install(const char *url);
int plugin_load(const char *name);
void plugin_list(void);

#endif
