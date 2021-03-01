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
mode_t permission(enum file_class c, enum file_permission p);

/**
 * @brief Checks if a permission is set on a mode_t
 * @param m mode to check
 * @param c class permission to check
 * @param p permission to check
 * @return true if permission is set in met
 */
bool check_permission(mode_t m, enum file_class c, enum file_permission p);

/**
 * @brief Set a permission
 * @param m mode to set
 * @param c class permission to check
 * @param p permission to check
 * @return m with permission set
 */
mode_t set_permission(mode_t m, enum file_class c, enum file_permission p);

/**
 * @brief Check if path is a directory
 * @param path filepath to check
 * @return -1 if path doesn't exist, 0 if it is a file, >0 if it a directory
 */
int is_directory(const char * path);
