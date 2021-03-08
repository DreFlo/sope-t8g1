#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>

typedef struct {
    bool owner;
    bool group;
    bool other;
}   UsersAccess;

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
 * @brief Subtracts a permission [NOT IMPLEMENTED]
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
