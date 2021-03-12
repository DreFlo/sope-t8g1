#include "xmod_sig_handlers.h"

void sigint_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGINT");
    char ans;
    snprintf(msg, 1024, "%d ; %s ; %u ; %u\n", proc_id, proc_start_path, nftot, nfmod);
    write(STDOUT_FILENO, msg, strlen(msg) + 1);
    //kill_all_children(SIGINT);
    if (main_proc) {
        sleep(1);
        do
        {
            write(STDOUT_FILENO, "Would you like to terminate? [Y/n] ", 36);
            read(STDIN_FILENO, &ans, 1);
        } while (ans != 'Y' && ans != 'n');

        switch (ans) {
        case 'Y':
            if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGQUIT", proc_id);
            raise(SIGQUIT);
            break;
        case 'n':
            if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGCONT", proc_id);
            raise(SIGCONT);
            break;
        }
    } else {
        pause();
    }
    return;
}

void sigquit_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGQUIT");
    kill_all_children(SIGQUIT);
    exit_plus(1);
}

void sigcont_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGCONT");
    kill_all_children(SIGCONT);
    return;
}
