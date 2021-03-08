#ifndef _UTILS_H_
#define _UTILS_H_

#include <time.h>
#include <stdarg.h>
#include "xmod_macros.h"
#include "xmod_sig_handlers.h"

extern bool main_proc;
extern pid_t proc_id;
extern char * proc_start_path;
extern unsigned int nftot, nfmod;

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
/**
 * @brief Struct with permissions to change
 */
/**
 * @brief Enumerates different types of events
 */
enum event {PROC_CREAT, PROC_EXIT, SIGNAL_RECV, SIGNAL_SENT, FILE_MODF};


/**
 * @brief Creates or cleans the file to store the execution registers
 * @param path environmental variable "LOG_FILENAME"
 * @return int 0 on success, non-zero otherwise
 */
int start_log_file(char *path);

/**
 * @brief Writes execution register in file
 * @param argc number of arguments
 * @param ... Next three: File path, enum that represents the type of event that occured and time in which the main process started.
 *            Rest of arguments differ for each event.
 * @return int 
 */
int write_exec_register(int argc, ...);

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
 * @brief Sets child proccess info, should be called everytime a new child process is created
 */
void set_child_proccess_info();

#endif //_UTILS_H_