#include "xmod_sig_handlers.h"

void sigint_handler() {
    char ans;
    if (!sig_no) printf("%d ; %s ; %u ; %u\n", proc_id, proc_start_path, nftot, nfmod);
    if (main_proc) {
        if (!sig_no) {
            sig_no++;
            kill(-proc_id, SIGINT);
            sleep(1);
            do
            {
                printf("Would you like to terminate? [Y/n] ");
                ans = getchar();
            } while (ans != 'Y' && ans != 'n');

            switch (ans) {
                case 'Y':
                    kill(-proc_id, SIGQUIT);
                    break;
                case 'n':
                    kill(-proc_id, SIGCONT);
                    break;
            } 
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
