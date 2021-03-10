CC=g++
CFLAGS=-O3

vaccineMonitor: vaccineMonitorMain parsing.o parsing_utils.o record_list.o disease_list.o bloom_filter.o skip_list.o
	$(CC) -o bin/vaccineMonitor object/*.o $(CFLAGS)

vaccineMonitorMain:
	$(CC) -c -o object/vaccine_monitor_main.o src/vaccineMonitor.cc $(CFLAGS)

parsing.o:
	$(CC) -c -o object/parsing.o src/utils/parsing.cc $(CFLAGS)

parsing_utils.o:
	$(CC) -c -o object/parsing_utils.o src/utils/parsing_utils.cc $(CFLAGS)

record_list.o:
	$(CC) -c -o object/record_list.o src/data_structures/record_list.cc $(CFLAGS)

disease_list.o:
	$(CC) -c -o object/disease_list.o src/data_structures/disease_list.cc $(CFLAGS)

bloom_filter.o:
	$(CC) -c -o object/bloom_filter.o src/data_structures/bloom_filter.cc $(CFLAGS)

skip_list.o:
	$(CC) -c -o object/skip_list.o src/data_structures/skip_list.cc $(CFLAGS)

# clean
clean:
	rm bin/* object/*.o
