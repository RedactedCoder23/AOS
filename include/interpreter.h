#ifndef INTERPRETER_H
#define INTERPRETER_H

typedef void (*command_handler)(int argc, char **argv);

void add_command(const char *name, command_handler handler);
void cmd_hello_wrapper(int argc, char **argv);
void cmd_bye_wrapper(int argc, char **argv);
void ui_graph_wrapper(int argc, char **argv);
void cmd_ui_graph_wrapper(int argc, char **argv);
void cmd_mem_alloc_wrapper(int argc, char **argv);
void cmd_mem_free_wrapper(int argc, char **argv);
void cmd_fs_open_wrapper(int argc, char **argv);
void cmd_fs_write_wrapper(int argc, char **argv);
void cmd_fs_read_wrapper(int argc, char **argv);
void cmd_fs_close_wrapper(int argc, char **argv);
void cmd_fs_ls_wrapper(int argc, char **argv);
void cmd_ai_prompt_wrapper(int argc, char **argv);
void cmd_ai_profile_wrapper(int argc, char **argv);
void cmd_br_create_wrapper(int argc, char **argv);
void cmd_br_list_wrapper(int argc, char **argv);
void cmd_br_stop_wrapper(int argc, char **argv);
void cmd_br_delete_wrapper(int argc, char **argv);
void init_command_map();

void cmd_br_vm_create_wrapper(int argc, char **argv);
void cmd_br_vm_list_wrapper(int argc, char **argv);
void cmd_br_vm_switch_wrapper(int argc, char **argv);
void cmd_br_vm_stop_wrapper(int argc, char **argv);
void cmd_plugin_install_wrapper(int argc, char **argv);
void cmd_plugin_list_wrapper(int argc, char **argv);
void cmd_plugin_load_wrapper(int argc, char **argv);
void cmd_plugin_unload_wrapper(int argc, char **argv);
void cmd_br_peer_add_wrapper(int argc, char **argv);
void cmd_br_sync_wrapper(int argc, char **argv);
void cmd_br_sync_all_wrapper(int argc, char **argv);
void cmd_ai_service_create_wrapper(int argc, char **argv);
void cmd_ai_service_monitor_wrapper(int argc, char **argv);
void cmd_policy_load_wrapper(int argc, char **argv);
void cmd_policy_check_wrapper(int argc, char **argv);

#endif
