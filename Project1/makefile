CC=g++
CFLAGS=-O3

vaccineMonitor: vaccineMonitorMain parsing.o parsing_utils.o utils.o record_list.o virus_list.o bloom_filter.o skip_list.o hash_table.o
	$(CC) -o bin/vaccineMonitor object/*.o $(CFLAGS)

vaccineMonitorMain:
	$(CC) -c -o object/vaccine_monitor_main.o core/vaccineMonitor.cc $(CFLAGS)

parsing.o:
	$(CC) -c -o object/parsing.o core/utils/parsing.cc $(CFLAGS)

parsing_utils.o:
	$(CC) -c -o object/parsing_utils.o core/utils/parsing_utils.cc $(CFLAGS)

utils.o:
	$(CC) -c -o object/utils.o core/utils/utils.cc $(CFLAGS)

record_list.o:
	$(CC) -c -o object/record_list.o core/data_structures/record_list.cc $(CFLAGS)

virus_list.o:
	$(CC) -c -o object/virus_list.o core/data_structures/virus_list.cc $(CFLAGS)

bloom_filter.o:
	$(CC) -c -o object/bloom_filter.o core/data_structures/bloom_filter.cc $(CFLAGS)

skip_list.o:
	$(CC) -c -o object/skip_list.o core/data_structures/skip_list.cc $(CFLAGS)

hash_table.o:
	$(CC) -c -o object/hash_table.o core/data_structures/hash_table.cc $(CFLAGS)

# clean
clean:
	rm -rf bin/* object/*.o
