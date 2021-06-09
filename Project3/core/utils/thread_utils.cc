#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <dirent.h>

#include "../../include/utils/utils.hpp"
#include "../../include/utils/parsing.hpp"
#include "../../include/utils/thread_utils.hpp"
#include "../../include/data_structures/indices.hpp"



/* global variables from monitorServer */
extern structures::CyclicBuffer* cyclic_buffer;
extern structures::RaceConditions race_cond;
extern ErrorHandler handler;

/* global variables of file */
static size_t num_parsed = 0;
static bool flag_exit = false;



void thread_utils::initialize_thread_variables(void)
{
    pthread_mutex_init(&race_cond.buffer_access, 0);
    pthread_mutex_init(&race_cond.data_structures_access, 0);
    pthread_mutex_init(&race_cond.counter_access, 0);
	pthread_cond_init(&race_cond.cond_non_empty, 0);
	pthread_cond_init(&race_cond.cond_non_full, 0);
	pthread_cond_init(&race_cond.counter_is_max, 0);
}



void thread_utils::destroy_thread_variables(void)
{
    pthread_cond_destroy(&race_cond.cond_non_empty);
	pthread_cond_destroy(&race_cond.cond_non_full);
	pthread_cond_destroy(&race_cond.counter_is_max);
	pthread_mutex_destroy(&race_cond.buffer_access);
	pthread_mutex_destroy(&race_cond.data_structures_access);
	pthread_mutex_destroy(&race_cond.counter_access);
}



void thread_utils::create_threads(pthread_t pthread_ids[], const uint16_t & num_threads, MonitorIndex* m_index)
{
    int ret = -1;
    for (size_t i = 0; i < num_threads; i++)
        if (ret = pthread_create(&pthread_ids[i], NULL, thread_utils::_thread_consumer, (void*) m_index))
            utils::strerror_exit("pthread_create() @ thread_utils::create_threads()", ret);
}



void thread_utils::wait_for_threads(pthread_t pthread_ids[], const uint16_t & num_threads)
{
    int ret = -1;
    for (size_t i = 0; i < num_threads; i++)
        if (ret = pthread_join(pthread_ids[i], NULL))
            utils::strerror_exit("pthread_create() @ thread_utils::wait_for_threads()", ret);
}




void* thread_utils::_thread_consumer(void* ptr)
{
    MonitorIndex* m_index = (MonitorIndex*) ptr;

    while (true)
    {
        /* get a country filepath from the cyclic buffer */
	    pthread_mutex_lock(&race_cond.buffer_access);
        while (cyclic_buffer->is_empty())
        {
            pthread_cond_wait(&race_cond.cond_non_empty, &race_cond.buffer_access);
            if (flag_exit)
            {
                pthread_mutex_unlock(&race_cond.buffer_access);
                pthread_exit(NULL);
            }
        }
        char* path = cyclic_buffer->remove();
        pthread_mutex_unlock(&race_cond.buffer_access);

        /* get information about the file to parse */
        std::string* country_ptr = m_index->country_ptr(parent_directory_name(path));
        std::string filepath = m_index->input->root_dir + std::string(path);

        /* parse it */
        parsing::dataset::parse_country_dataset(country_ptr, filepath, m_index, &race_cond.data_structures_access, handler);

        /* free the file name */
        delete[] path;

        /* cyclic buffer should not be full now */
        pthread_cond_signal(&race_cond.cond_non_full);

        /* update the counter with the number of parsed files and signal the main process */
        pthread_mutex_lock(&race_cond.counter_access);
        num_parsed++;
        pthread_mutex_unlock(&race_cond.counter_access);
        pthread_cond_signal(&race_cond.counter_is_max);
    }

    /* should never be executed */
    pthread_exit(NULL);
}



void thread_utils::produce(MonitorIndex* m_index)
{
    /* total data files that the program will encounter */
    size_t num_data_files = 0;

    /* for each country assigned to the monitorServer */
    for (size_t country_id = 0; country_id < m_index->input->num_countries; country_id++)
    {
        struct dirent **namelist;
        char country_path[257] = {0};
        sprintf(country_path, "%s/%s", m_index->input->root_dir.c_str(), m_index->input->countries[country_id].c_str());
        int num_files = scandir(country_path, &namelist, NULL, alphasort);
        num_data_files += num_files - 2;

        /* for each data file in it */
        for (size_t i = 0; i < num_files; i++)
        {
            if (strcmp(namelist[i]->d_name, ".") && strcmp(namelist[i]->d_name, ".."))
            {
                char country_slash_filename[258] = {0};
                sprintf(country_slash_filename, "/%s/%s", m_index->input->countries[country_id].c_str(), namelist[i]->d_name);

                /* insert the filename in the cyclic buffer */
	            pthread_mutex_lock(&race_cond.buffer_access);
                while (cyclic_buffer->is_full())
                    pthread_cond_wait(&race_cond.cond_non_full, &race_cond.buffer_access);
                cyclic_buffer->insert(country_slash_filename);
                pthread_mutex_unlock(&race_cond.buffer_access);

                /* cyclic buffer should not be empty now */
                pthread_cond_signal(&race_cond.cond_non_empty);

                /* insert the filename in the list with the filenames */
                std::string* _filename = new std::string(namelist[i]->d_name);
                m_index->files_per_country[country_id]->insert(_filename);
            }
            free(namelist[i]);
        }
        free(namelist);
    }

    /* wait until all files have been parsed */
    pthread_mutex_lock(&race_cond.counter_access);
    while (num_parsed < num_data_files)
        pthread_cond_wait(&race_cond.counter_is_max, &race_cond.counter_access);
    pthread_mutex_unlock(&race_cond.counter_access);
}



void thread_utils::produce_new(MonitorIndex* m_index, const std::string & country)
{
    /* get pointer of the specified country */
    const int country_id = m_index->country_id(std::string(country));
    std::string* country_ptr = &(m_index->input->countries[country_id]);

    /* total new data files that will be found */
    size_t num_data_files = 0;
    pthread_mutex_lock(&race_cond.counter_access);
    num_parsed = 0;
    pthread_mutex_unlock(&race_cond.counter_access);


    /* parse again the country directory and look for new files */
    char country_dir_path[256] = {0};
    sprintf(country_dir_path, "%s/%s", m_index->input->root_dir.c_str(), country_ptr->c_str());
    struct dirent **namelist;
    int num_files = scandir(country_dir_path, &namelist, NULL, alphasort);

    /* for each data file in that directory */
    for (size_t i = 0; i < num_files; i++)
    {
        std::string* filename = new std::string(namelist[i]->d_name);

        /* parse the files that haven't been seen for this country */
        if (*filename != "." && *filename != ".." && !m_index->files_per_country[country_id]->in(filename))
        {
            char country_slash_filename[258] = {0};
            sprintf(country_slash_filename, "/%s/%s", country_ptr->c_str(), namelist[i]->d_name);

            /* insert the filename in the cyclic buffer */
            pthread_mutex_lock(&race_cond.buffer_access);
            while (cyclic_buffer->is_full())
                pthread_cond_wait(&race_cond.cond_non_full, &race_cond.buffer_access);
            cyclic_buffer->insert(country_slash_filename);
            pthread_mutex_unlock(&race_cond.buffer_access);

            /* cyclic buffer should not be empty now */
            pthread_cond_broadcast(&race_cond.cond_non_empty);

            /* insert the filename in the list with the filenames */
            m_index->files_per_country[country_id]->insert(filename);
            num_data_files++;
        }
        else
        {
            delete filename;
        }
        free(namelist[i]);
    }
    free(namelist);

    /* wait until all files have been parsed */
    pthread_mutex_lock(&race_cond.counter_access);
    while (num_parsed < num_data_files)
        pthread_cond_wait(&race_cond.counter_is_max, &race_cond.counter_access);
    pthread_mutex_unlock(&race_cond.counter_access);
}



void thread_utils::exit_threads(void)
{
    /* set the flag to true and wake them up */
    flag_exit = true;
    pthread_cond_broadcast(&race_cond.cond_non_empty);
}
