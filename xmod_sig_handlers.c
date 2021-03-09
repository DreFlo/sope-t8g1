#include "xmod_sig_handlers.h"

int kill_all_children(int sig) {
    for (int i = 0; i < child_no; i++) {
        if (kill(children[i], sig) != 0) {
            return 1;
        }
        if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGINT", children[i]);
        
    }
    return 0;
}

void sigint_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGINT");
    char ans;
    printf("%d ; %s ; %u ; %u\n", proc_id, proc_start_path, nftot, nfmod);
    kill_all_children(SIGINT);
    if (main_proc) {
        sleep(1);
        do
        {
            printf("Would you like to terminate? [Y/n] ");
            ans = getchar();
        } while (ans != 'Y' && ans != 'n');

        switch (ans) {
        case 'Y':
            kill_all_children(SIGQUIT);
            if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGQUIT", proc_id);
            raise(SIGQUIT);
            break;
        case 'n':
            kill_all_children(SIGCONT);
            if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGCONT", proc_id);
            raise(SIGCONT);
            break;
        }
    } else {
        pause();
    }
}

void sigquit_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGQUIT");
    if (!main_proc) kill_all_children(SIGQUIT);
    wait(NULL);
    exit(1);
}

void sigcont_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGCONT");
    if (!main_proc) kill_all_children(SIGCONT);
    return;
}
