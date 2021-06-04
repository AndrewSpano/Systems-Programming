CC=g++
CFLAGS=-O3
LIBS=-lpthread


ALL: travelMonitor Monitor


########################################################################################
# travel monitor program

travelMonitor: travelMonitorMain parsing.o parsing_utils.o network_utils.o process_utils.o ipc.o setup.o bloom_filter.o skip_list.o virus_list.o queries.o utils.o
	$(CC) -o bin/travelMonitorClient object/travelMonitor_objects/*.o object/common_objects/*.o $(CFLAGS) $(LIBS)

travelMonitorMain:
	$(CC) -c -o object/travelMonitor_objects/travel_monitor_main.o core/travelMonitorClient.cc $(CFLAGS) $(LIBS)

parsing.o:
	$(CC) -c -o object/common_objects/parsing.o core/utils/parsing.cc $(CFLAGS) $(LIBS)

parsing_utils.o:
	$(CC) -c -o object/common_objects/parsing_utils.o core/utils/parsing_utils.cc $(CFLAGS) $(LIBS)

network_utils.o:
	$(CC) -c -o object/common_objects/network_utils.o core/utils/network_utils.cc $(CFLAGS) $(LIBS)

process_utils.o:
	$(CC) -c -o object/common_objects/process_utils.o core/utils/process_utils.cc $(CFLAGS) $(LIBS)

ipc.o:
	$(CC) -c -o object/common_objects/ipc.o core/ipc/ipc.cc $(CFLAGS) $(LIBS)

setup.o:
	$(CC) -c -o object/common_objects/setup.o core/ipc/setup.cc $(CFLAGS) $(LIBS)

bloom_filter.o:
	$(CC) -c -o object/common_objects/bloom_filter.o core/data_structures/bloom_filter.cc $(CFLAGS) $(LIBS)

skip_list.o:
	$(CC) -c -o object/common_objects/skip_list.o core/data_structures/skip_list.cc $(CFLAGS) $(LIBS)

virus_list.o:
	$(CC) -c -o object/common_objects/virus_list.o core/data_structures/virus_list.cc $(CFLAGS) $(LIBS)

queries.o:
	$(CC) -c -o object/common_objects/queries.o core/ipc/queries.cc $(CFLAGS) $(LIBS)

utils.o:
	$(CC) -c -o object/common_objects/utils.o core/utils/utils.cc $(CFLAGS) $(LIBS)


########################################################################################
# monitor program

Monitor: MonitorMain parsing.o parsing_utils.o
	$(CC) -o bin/monitorServer object/Monitor_objects/*.o object/common_objects/*.o $(CFLAGS) $(LIBS)

MonitorMain:
	$(CC) -c -o object/Monitor_objects/monitor_main.o core/monitorServer.cc $(CFLAGS) $(LIBS)


# clean
clean:
	rm -rf bin/* object/common_objects/*.o object/Monitor_objects/*.o object/travelMonitor_objects/*.o logfiles/log_file.*
