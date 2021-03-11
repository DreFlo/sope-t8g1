#ifndef XMOD_UTILS_H_
#define XMOD_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <stdarg.h>
#include "xmod_macros.h"

extern bool main_proc;
extern pid_t proc_id;
extern char * proc_start_path;
extern unsigned int nftot;
extern unsigned int nfmod;

extern pid_t children[256];
extern int child_no;

extern char* log_path;
extern clock_t begin;
extern bool log_filename;

extern int s_argc;
extern char ** s_argv;
extern char ** s_envp;

/**
 * @brief Struct with user settable flags
 */
typedef struct {
    bool v;                                                 /* Verbose */ 
    bool c;                                                 /* Changes */
    bool r;                                                 /* Recursive */
}flag_t, * flag_p;

/**
 * @brief Struct with accesses to change 
 */
typedef struct {
    bool owner;
    bool group;
    bool other;
}   UsersAccess;

/**
 * @brief Struct with permissions to change
 */
typedef struct {
    bool read;
    bool write;
    bool execute;
}   Permissions;

/**
 * @brief Enumerates various file classes
 */
enum file_class { FILE_CLASS_OWNER, FILE_CLASS_GROUP, FILE_CLASS_OTHER };

/**
 * @brief Enumerates various file permissions
 */
enum file_permission { FILE_PERMISSION_READ, FILE_PERMISSION_WRITE, FILE_PERMISSION_EXEC };

/**
 * @brief Enumerates different types of events
 */
enum event {PROC_CREAT, PROC_EXIT, SIGNAL_RECV, SIGNAL_SENT, FILE_MODF};

/**
 * @brief Get a bitmask for a permission on a certain class
 * @param c class for which permission will be set
 * @param p permission to be set
 * @return mode_t bitmask
 */
mode_t bitmask_permission(enum file_class c, enum file_permission p);

/**
 * @brief Checks if a permission is set on a mode_t
 * @param m mode to check
 * @param c class permission to check
 * @param p permission to check
 * @return true if permission is set in met
 */
bool check_permission(mode_t m, enum file_class c, enum file_permission p);

/**
 * @brief Adds a permission
 * @param m mode to set
 * @param c class permission to check
 * @param p permission to check
 * @return m with permission set
 */
mode_t add_permission(mode_t m, enum file_class c, enum file_permission p);

/**
 * @brief Subtracts a permission
 * @param m mode to set
 * @param c class permission to check
 * @param p permission to check
 * @return m with permission unset
 */
mode_t subtract_permission(mode_t m, enum file_class c, enum file_permission p);

/**
 * @brief Check if path is a directory
 * @param path filepath to check
 * @return -1 if path doesn't exist, 0 if it is a file, >0 if it a directory
 */
int is_directory(const char * path);

/**
 * @brief Gets new file mode from string and stores it in new_mode [NOT IMPLEMENTED]
 * @param rx string to parse
 * @param new_mode pointer to mode_t where info will be stored
 * @param old_mode current file mode
 * @return 0 on success, non-zero otherwise
 */
int get_mode_from_string(const char * rx, mode_t * new_mode, const mode_t old_mode);

/**
 * @brief Get the umask object
 * @return Current system umask
 */
mode_t get_umask();

/**
 * @brief Gets file mode from mode_t and converts it into permissions sequence buf 
 * @param mode file mode
 * @param buf pointer to char where permissions sequence will be stored
 */
void str_mode(mode_t mode, char * buf);

/**
 * @brief Removes the begining of envp until '='
 * @param envp environmental variable to change
 */
void rem_beg_envp(char* envp);

/**
 * @brief Creates or cleans the file to store the execution registers
 * @return int 0 on success, non-zero otherwise
 */
int start_log_file();

/**
 * @brief Writes execution register in file
 * @param argc number of arguments
 * @param ... Second argument: enum that represents the type of event that occured.
 *            Rest of arguments differ for each event.
 * @return int 
 */
int write_exec_register(int argc, ...);

/**
 * @brief Registers new child pid in children. To be run by the parent after every fork.
 * @param pid Child pid to register
 */
void register_new_child(pid_t pid);

/**
 * @brief Sets child proccess info, should be called everytime a new child process is created
 * @param path Process main path
 */
void set_child_proccess_info(char * path);

/**
 * @brief Changes mode of a single file or directory, prints info on screen according to flags
 * @param path path of file or directory to change
 * @param new_mode new path mode
 * @param old_mode old path mode
 * @param flags user set flags
 */
void xmod(const char * path, const mode_t new_mode, const mode_t old_mode, const flag_t flags);

/**
 * @brief Recursively changes mode of all files and sub-directories in path
 * @param path path of file or directory to change
 * @param dir pointer to directory to cycle through
 * @param new_mode new path mode
 * @param old_mode old path mode
 * @param flags user set flags 
 */
void recursive_xmod(char * path, DIR * dir, const mode_t new_mode, const mode_t old_mode, const flag_t flags);

/**
 * @brief Kills all children of this process
 * @param sig signal to kill with
 * @return 0 on success (at least one children killed), 1 otherwise
 */
int kill_all_children(int sig);

/**
 * @brief Waits for all child processes to terminate 
 */
void wait_all_children();

/**
 * @brief Calls the exit() function and writes the execution register if needed
 * @param status exit status
 */
void exit_plus(int status);

#endif // XMOD_UTILS_H_
