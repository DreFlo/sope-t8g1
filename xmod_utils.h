extern unsigned int nftot;
extern unsigned int nfmod;

extern pid_t children[128];
extern int child_no;

extern char* log_path;
extern clock_t begin;
extern bool log_filename;
int start_log_file();
 * @param ... Second argument: enum that represents the type of event that occured.
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
