CC=g++
CFLAGS=-O3

vaccineMonitor: vaccineMonitorMain parsing.o parsing_utils.o
	$(CC) -o bin/vaccineMonitor object/vaccine_monitor_main.o object/parsing.o object/parsing_utils.o $(CFLAGS)

vaccineMonitorMain:
	$(CC) -c -o object/vaccine_monitor_main.o src/vaccineMonitor.cc $(CFLAGS)

parsing.o:
	$(CC) -c -o object/parsing.o src/utils/parsing.cc $(CFLAGS)

parsing_utils.o:
	$(CC) -c -o object/parsing_utils.o src/utils/parsing_utils.cc $(CFLAGS)

# clean
clean:
	rm bin/* object/*.o
