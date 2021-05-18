#ifndef _TRAVEL_MONITOR_SIG_HANDLE
#define _TRAVEL_MONITOR_SIG_HANDLE


void initialize_signal_handlers(void);
void block_sigchld(void);
void unblock_sigchld(void);
void block_sigint_sigquit_sigchld(void);
void unblock_sigint_sigquit_sigchld(void);



#endif