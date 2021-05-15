#include <iostream>
#include <csignal>
#include <unistd.h>

#include "../../include/signal_handlers/monitor_signal_handling.hpp"
#include "../../include/data_structures/indices.hpp"
#include "../../include/utils/structures.hpp"
#include "../../include/utils/errors.hpp"
#include "../../include/ipc/queries.hpp"


sigset_t target_signals_set;


extern MonitorIndex* m_index;
extern structures::CommunicationPipes* pipes;
extern ErrorHandler handler;



static void handle_sigint_sigquit(int sigtype)
{ m_index->logger->write_to_logfile(); }


static void handle_sigusr1(int sigtype)
{ ipc::monitor::queries::add_vaccination_records(m_index, pipes, handler); }



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

    /* do the same for SIGUSR1 */
    action.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &action, NULL);

    /* singals will be blocked when a query is being executed, and received later */
    sigemptyset(&target_signals_set);
    sigaddset(&target_signals_set, SIGINT);
    sigaddset(&target_signals_set, SIGQUIT);
    sigaddset(&target_signals_set, SIGUSR1);
}


void block_sigint_sigquit_sigusr1(void)
{ sigprocmask(SIG_BLOCK, &target_signals_set, NULL); }


void unblock__siging_sigquit_sigusr1(void)
{ sigprocmask(SIG_UNBLOCK, &target_signals_set, NULL); }
