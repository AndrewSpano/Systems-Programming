CC=g++
CFLAGS=-O3


ALL: travelMonitor Monitor


########################################################################################
# travel monitor program

travelMonitor: travelMonitorMain parsing.o parsing_utils.o process_utils.o comm_utils.o bloom_filter.o skip_list.o virus_list.o queries.o
	$(CC) -o bin/travelMonitor object/travelMonitor_objects/*.o object/common_objects/*.o $(CFLAGS)

travelMonitorMain:
	$(CC) -c -o object/travelMonitor_objects/travel_monitor_main.o core/travelMonitor.cc $(CFLAGS)

parsing.o:
	$(CC) -c -o object/common_objects/parsing.o core/utils/parsing.cc $(CFLAGS)

parsing_utils.o:
	$(CC) -c -o object/common_objects/parsing_utils.o core/utils/parsing_utils.cc $(CFLAGS)

process_utils.o:
	$(CC) -c -o object/common_objects/process_utils.o core/utils/process_utils.cc $(CFLAGS)

comm_utils.o:
	$(CC) -c -o object/common_objects/comm_utils.o core/utils/comm_utils.cc $(CFLAGS)

bloom_filter.o:
	$(CC) -c -o object/common_objects/bloom_filter.o core/data_structures/bloom_filter.cc $(CFLAGS)

skip_list.o:
	$(CC) -c -o object/common_objects/skip_list.o core/data_structures/skip_list.cc $(CFLAGS)

virus_list.o:
	$(CC) -c -o object/common_objects/virus_list.o core/data_structures/virus_list.cc $(CFLAGS)

queries.o:
	$(CC) -c -o object/common_objects/queries.o core/utils/queries.cc $(CFLAGS)


########################################################################################
# monitor program

Monitor: MonitorMain parsing.o parsing_utils.o
	$(CC) -o bin/Monitor object/Monitor_objects/*.o object/common_objects/*.o $(CFLAGS)

MonitorMain:
	$(CC) -c -o object/Monitor_objects/monitor_main.o core/Monitor.cc $(CFLAGS)



# clean
clean:
	rm -rf bin/* object/*.o
