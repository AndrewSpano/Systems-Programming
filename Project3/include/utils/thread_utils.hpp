#ifndef _THREAD_UTILS
#define _THREAD_UTILS



namespace thread_utils
{
    void initialize_thread_variables(void);
    void destroy_thread_variables(void);

    void create_threads(pthread_t pthread_ids[], const uint16_t & num_threads, MonitorIndex* m_index);
    void wait_for_threads(pthread_t pthread_ids[], const uint16_t & num_threads);

    void* _thread_consumer(void* ptr);
    void produce(MonitorIndex* m_index);
    void produce_new(MonitorIndex* m_index, const int & country_id);

    void exit_threads(void);
}



#endif