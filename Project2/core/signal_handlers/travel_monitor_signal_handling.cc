#include <iostream>
#include <csignal>
#include <unistd.h>

#include "../../include/signal_handlers/travel_monitor_signal_handling.hpp"
#include "../../include/data_structures/indices.hpp"
#include "../../include/utils/process_utils.hpp"
#include "../../include/utils/structures.hpp"
#include "../../include/ipc/queries.hpp"


sigset_t target_signals_set;


extern travelMonitorIndex* tm_index;
extern structures::CommunicationPipes* pipes;
extern pid_t* monitor_pids;



static void handle_sigint_sigquit(int sigtype)
{
    /* kill all the monitors, then make sure they have died, write to the logfiles and cleanup the allocated memory */
    process_utils::travel_monitor::kill_minitors_and_wait(monitor_pids, tm_index);
    tm_index->logger->write_to_logfile();
    process_utils::travel_monitor::cleanup(tm_index, pipes, monitor_pids);

    exit(EXIT_SUCCESS);
}


static void handle_sigchld(int sigtype)
{
    /* do not handle the case where the child processes died because the parent process killed them when exiting */
    if (tm_index->has_sent_sigkill)
        return;

    /* handle the signal */
    ipc::travel_monitor::queries::handle_sigchld(tm_index, pipes, monitor_pids);
}



void initialize_signal_handlers(void)
{
    /* sigaction structure used to specify handlers and their behavior */
    struct sigaction action;
    /* set all the fields to 0 so that valgrind does not complain about the action.flags uninitialized value */
    memset(&action, 0, sizeof(struct sigaction));

    /* specify the handler for SIGINT and SIGQUIT, and block other signals when the handlers are being executed */
    action.sa_handler = handle_sigint_sigquit;
    sigfillset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);

    /* do the same for SIGCHLD */
    action.sa_handler = handle_sigchld;
    sigaction(SIGCHLD, &action, NULL);

    /* singals will be blocked when a query is being executed, and received later */
    sigemptyset(&target_signals_set);
    sigaddset(&target_signals_set, SIGINT);
    sigaddset(&target_signals_set, SIGQUIT);
    sigaddset(&target_signals_set, SIGCHLD);
}


void block_sigint_sigquit_sigchld(void)
{ sigprocmask(SIG_BLOCK, &target_signals_set, NULL); }


void unblock_sigint_sigquit_sigchld(void)
{ sigprocmask(SIG_UNBLOCK, &target_signals_set, NULL); }
