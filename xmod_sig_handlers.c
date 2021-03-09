#include "xmod_sig_handlers.h"

int sig_no = 0;                  

void sigint_handler() {
    if (log_filename) write_exec_register(2, SIGNAL_RECV, "SIGINT");
    char ans;
    if (!sig_no) printf("%d ; %s ; %u ; %u\n", proc_id, proc_start_path, nftot, nfmod);
    if (main_proc) {
        if (!sig_no) {
            sig_no++;
            if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGINT", proc_id);
            kill(-proc_id, SIGINT);
            sleep(1);
            do
            {
                printf("Would you like to terminate? [Y/n] ");
                ans = getchar();
            } while (ans != 'Y' && ans != 'n');

            switch (ans) {
                case 'Y':
                    if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGQUIT", proc_id);
                    kill(-proc_id, SIGQUIT);
                    break;
                case 'n':
                    if (log_filename) write_exec_register(3, SIGNAL_SENT, "SIGCONT", proc_id);
                    kill(-proc_id, SIGCONT);
                    break;
            }
        }
        else {
            sig_no = 0;
        }
    }
    else {
        pause();
    }
    return;
}

void sigquit_handler() {
    wait(NULL);
    exit(1);
}

void sigcont_handler() {
    return;
}
