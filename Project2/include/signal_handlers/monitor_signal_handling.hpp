#ifndef _MONITOR_SIG_HANDLE
#define _MONITOR_SIG_HANDLE


void initialize_signal_handlers(void);
void block_sigint_sigquit_sigusr1(void);
void unblock__siging_sigquit_sigusr1(void);



#endif