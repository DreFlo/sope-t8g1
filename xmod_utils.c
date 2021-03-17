#include "xmod_utils.h"
#include <signal.h>

mode_t bitmask_permission(enum file_class c, enum file_permission p)
{
    return 1 << ((2 - p) + (2 - c) * 3);
}

bool check_permission(mode_t m, enum file_class c, enum file_permission p)
{
    return m & bitmask_permission(c, p);
}

mode_t add_permission(mode_t m, enum file_class c, enum file_permission p)
{
    return m | bitmask_permission(c, p);
}

mode_t subtract_permission(mode_t m, enum file_class c, enum file_permission p)
{
    return m & ~bitmask_permission(c, p);
}

int is_directory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return -1;
    return S_ISDIR(path_stat.st_mode);
}

int get_mode_from_string(const char *rx, mode_t *new_mode, const mode_t old_mode)
{

    // Check for users and permissions
    UsersAccess ua = {false, false, false};
    Permissions perm = {false, false, false};

    if (strlen(rx) < 3 || strlen(rx) > 5)
        return 1;

    // Check target
    switch (rx[0])
    {
    case 'a':
        ua.owner = true;
        ua.group = true;
        ua.other = true;
        break;
    case 'u':
        ua.owner = true;
        break;
    case 'g':
        ua.group = true;
        break;
    case 'o':
        ua.other = true;
        break;
    default:
        return 1;
    }

    // Operation
    char op = rx[1];

    // Check for permissions
    for (int j = 2; rx[j] != '\0'; j++)
    {
        // Set permissions
        switch (rx[j])
        {
        case 'r':
            if (perm.read == true)
                return 1;
            perm.read = true;
            break;
        case 'w':
            if (perm.write == true)
                return 1;
            perm.write = true;
            break;
        case 'x':
            if (perm.execute == true)
                return 1;
            perm.execute = true;
            break;
        default:
            return 1;
        }
    }

    // Change the new_mode
    switch (op)
    {
    case '+':
        *new_mode = old_mode;

        if (ua.owner)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
        }
        if (ua.group)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
        }
        if (ua.other)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
        }
        break;

    case '-':
        *new_mode = old_mode;

        if (ua.owner)
        {
            if (perm.read)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
        }
        if (ua.group)
        {
            if (perm.read)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
        }
        if (ua.other)
        {
            if (perm.read)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
        }
        break;

    case '=':
        if (ua.owner)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
        }
        else
        {
            for (int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++)
                if (check_permission(old_mode, FILE_CLASS_OWNER, i))
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, i);
        }
        if (ua.group)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
        }
        else
        {
            for (int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++)
                if (check_permission(old_mode, FILE_CLASS_GROUP, i))
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, i);
        }
        if (ua.other)
        {
            if (perm.read)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
            if (perm.write)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
            if (perm.execute)
                *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
        }
        else
        {
            for (int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++)
                if (check_permission(old_mode, FILE_CLASS_OTHER, i))
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, i);
        }
        break;
    default:
        return 1;
    }

    return 0;
}

mode_t get_umask()
{
    mode_t mask = umask(0000);
    umask(mask);
    return mask;
}

void str_mode(mode_t mode, char *buf)
{
    const char permissions[] = "rwxrwxrwx";
    for (size_t i = 0; i < 9; i++)
    {
        buf[i] = (mode & (1 << (8 - i))) ? permissions[i] : '-';
    }
    buf[9] = '\0';
}

int start_log_file()
{
    char *folder_path = (char *) malloc(strlen(log_path));
    char *file_path = (char * )malloc(strlen(log_path) + 4 * sizeof(char));

    switch (is_directory(log_path))
    {
    case -1:;
        for (size_t i = strlen(log_path) - 1; i >= 0; i--)
        {
            if (log_path[i] == '/')
            {
                memmove(folder_path, log_path, i + 1);
                break;
            }
        }
        if (is_directory(folder_path) == 1)
        {
            break;
        }
        else
        {
            return -1;
        }
        break;
    case 0:;
        break;
    default:;
        /**
            char* file_name = (char*) malloc(strlen(log_path));
            file_name = "logs";
            if(log_path[strlen(log_path) - 1] != '/'){
                char* add = "/";
                strcat(log_path, add);
            }
            strcat(log_path, file_name);*/
        return -1;
    }

    int file = open(log_path, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    close(file);

    return 0;
}

int write_exec_register(int argc, ...)
{
    sem_wait(semaphore);

    va_list args;
    va_start(args, argc);

    enum event ev = va_arg(args, enum event);

    char *event_s;
    char *info = malloc(2048);

    switch (ev)
    {
        case PROC_CREAT: //(..., char* cmd_args)
            event_s = "PROC_CREAT";
            char* arg = va_arg(args, char*);
            snprintf(info, 2048, "%s", arg);
            break;
        case PROC_EXIT: //(..., int exit)
            event_s = "PROC_EXIT";
            int exit = va_arg(args, int);
            snprintf(info, 100, "%d", exit);
            break;
        case SIGNAL_RECV: //(..., char* signal)
            event_s = "SIGNAL_RECV";
            char* signal = va_arg(args, char*);
            snprintf(info, 100, "%s", signal);
            break;
        case SIGNAL_SENT: //(..., char* signal, pid_t pid)
            event_s = "SIGNAL_SENT";
            char* sig = va_arg(args, char*);
            pid_t pid = va_arg(args, pid_t);
            snprintf(info, 100, "%s : %d", sig, pid);
            break;
        case FILE_MODF: //(..., char* path, mode_t old, mode_t new)
            event_s = "FILE_MODF";
            
            char* path = (char*) malloc(2000);
            path = va_arg(args, char*);

            info = realloc(info, strlen(path) * sizeof(char) + 1000);

            mode_t old = va_arg(args, mode_t);
            mode_t new = va_arg(args, mode_t);

            snprintf(info, strlen(path) * sizeof(char) + 100, "%s : 0%o : 0%o", path, old, new);
            break;
    }
    double instant = (double)(clock() - begin) / 1000;

    char line[strlen(info) * sizeof(char) + 100];

    snprintf(line, strlen(info) * sizeof(char) + 100, "%d ; %fms ; %s ; %s\n", getpid(), instant, event_s, info);

    int file = open(log_path, O_WRONLY | O_APPEND);

    write(file, line, strlen(line));

    close(file);
    
    va_end(args);

    sem_post(semaphore);
    return 0;
}

void register_new_child(pid_t pid)
{
    children[child_no] = pid;
    child_no++;
}

void xmod(const char *path, const mode_t new_mode, const mode_t old_mode, const flag_t flags)
{

    nftot++;

    int ret = chmod(path, new_mode);

    nfmod++;

    if (log_filename && new_mode != old_mode)
        write_exec_register(4, FILE_MODF, path, old_mode, new_mode);

    // Get mode strings
    char buf1[10], buf2[10];
    str_mode(old_mode, buf1);
    str_mode(new_mode, buf2);

    // Print messages if flags are set

    if ((flags.v || flags.c) && new_mode != old_mode)
    {
        printf("mode of '%s' changed from %.4o ('%s') to %.4o ('%s')\n", path, old_mode, buf1, new_mode, buf2);
    }

    else if ((flags.v && !flags.c) && new_mode == old_mode)
    {
        printf("mode of '%s' retained as %.4o ('%s')\n", path, new_mode, buf1);
    }
}

void recursive_xmod(char *path, DIR *dir, const mode_t new_mode, const mode_t old_mode, const flag_t flags)
{
    struct dirent *dir_struct; /* Pointer to current directory struct */
    char new_path[1024];       /* Path string for each thing in directory */
    struct stat path_stat;     /* Status of new_path */
    pid_t id;                  /* For multi-proccessing */

    while ((dir_struct = readdir(dir)) != NULL)
    {
        // ignore current folder and parent folder
        if (!strcmp(dir_struct->d_name, ".") || !strcmp(dir_struct->d_name, ".."))
            continue;

        // get current path
        snprintf(new_path, strlen(path) + strlen(dir_struct->d_name) + 1, "%s%s", path, dir_struct->d_name);

        // get current path status
        stat(new_path, &path_stat);

        // if dĩrectory change modes in new process, else change mode
        if (S_ISDIR(path_stat.st_mode))
        {
            switch (id = fork())
            {
            case -1:
                perror("fork:");
                exit_plus(EXIT_FAILURE);
            case 0:;
                struct timespec t = {0, 100000000};
                nanosleep(&t, NULL);
                s_argv[s_argc - 1] = new_path;
                execve(PROGRAM_NAME, s_argv, s_envp);
            default:
                register_new_child(id);
                nftot++;
                break;
            }
        }
        else if (S_ISREG(path_stat.st_mode))
        {
            xmod(new_path, new_mode, old_mode, flags);
        }
    }
    return;
}

int kill_all_children(int sig)
{
    if (child_no != 0)
    {
        int ret = 1;
        for (int i = 0; i < child_no; i++)
        {
            if (kill(children[i], sig) == 0)
            {
                ret = 0;
            }
            if (log_filename)
                write_exec_register(3, SIGNAL_SENT, "SIGINT", children[i]);
        }
        return ret;
    }
    return 0;
}

void wait_all_children()
{
    for (int i = 0; i < child_no; i++)
    {
        if (waitpid(children[i], NULL, 0) == -1 && errno == EINTR)
        {
            wait_all_children();
            break;
        }
    }
}

void exit_plus(int status)
{
    //if(main_proc) raise(SIGINT);
    wait_all_children();
    if (log_filename)
        write_exec_register(2, PROC_EXIT, status);

    sem_close(semaphore);
    if (main_proc)
        sem_unlink("/semaphore");

    exit(status);
}

bool check_octal_mode_format(char * mod) {
    // Check if it in OCTALMODE
    bool err = false;
    for(int i = 0; i < strlen(mod); i++) {
        if(mod[i] < '0' || mod[i] > '7') err = true;  
    }
    return mod != NULL && strlen(mod) > 0 && strlen(mod) <= 4 && mod[0] == '0' && !err;
}
