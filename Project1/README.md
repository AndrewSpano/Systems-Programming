# Vaccine Monitor

This is the fist out of three projects of the [Systems Programming](http://cgi.di.uoa.gr/~antoulas/k24/) course offered by the [Department of Informatics and Telecommunications](https://www.di.uoa.gr/en) of the [National and Kapodistrian University of Athens]((https://en.uoa.gr/)). It's purpose is to build a complete system that will be used to monitor the Vaccination information of different viruses for evey citizen (Record). The system is composed of various Data Structures ([Linked Lists](https://en.wikipedia.org/wiki/Linked_list), [Bloom Filters](https://en.wikipedia.org/wiki/Bloom_filter), [Hash Tables](https://en.wikipedia.org/wiki/Hash_table), [Skip Lists](https://en.wikipedia.org/wiki/Skip_list)) implemented from scratch, that help reduce query times to sub-linear.
<br> </br>


# Repository Structure

- [bin](bin): Contains the [executable file](bin/vaccineMonitor) produced from compilation.
- [object](object): Contains object files (*.o) produced from compilation.
- [include](include): Contains all the header files for the project. It is divided in two sub-directories:
    - [data_structures](include/data_structures): Contains the Header files for all the data structures used by the system.
    - [utils](include/utils): Contains header files for different utility functions.
- [core](core): Contains the core source code of the project. It contains two sub-directories and the main source file:
    - [data_structures](core/data_structures): Contains implementations of the data structures defined in the correspnding header files.
    - [utils](core/utils): Contains implementations for all the utility functions.
    - [vaccineMonitor.cc](core/vaccineMonitor.cc): The main source file that combines all the existing code in the Vaccine Monitor system.
- [bash](bash): Contains the [bash script](bash/testFile.sh) used to create datasets, and a [tests directory](bash/tests) used to store the datasets created.
<br> </br>


# Implementation Details

The main idea behind the implementation is very simple.

A [Record](include/data_structures/record.hpp) instance is created for every citizen that gets inserted in the Database. Pointers to the Records are stored in a [RecordList](include/data_structures/record_list.hpp).

After that, a [VirusList](include/data_structures/virus_list.hpp) is created in order to manage all the different Viruses that are inserted in the database. For every Node (virus) of the VirusList, 1 [Bloom Filter](include/data_structures/bloom_filter.hpp) and 2 [Skip Lists](include/data_structures/skip_list.hpp) are created. The Bloom Filter assists in finding quickly citizens that have not been vaccinated. The Skip Lists are used to deterministically decide if a citizen has been vaccinated or not: 1 Skip List contains pointers to the vaccinated citizens (and the corresponding dates of vaccination), and the other contains pointers to non Vaccinated citizens (for that specific virus).

At the same time, a [Hash Table](include/data_structures/hash_table.hpp) is used to store all the countries inserted in the database and assign an ID to them (the ID is their number-of-insertion, i.e. if Greece is the first country to be inserted then it will have the ID equal to 0, if Italy is then inserted it will have ID 1 and so on).

These data structures are all grouped in one structure called [Index](include/data_structures/index.hpp), which basically consists the database of the system. It provides an interface between database and the main file.
<br> </br>


# Usage

First we have to create a Dataset
```shell
$ cd bash
$ chmod +x testFile.sh
$ ./testFile.sh viruses_filepath countries_filepath number_of_records allow_duplicates
```
where the parameters are:
- viruses_filepath: The path to a file containing virus names, like [this one](bash/data/viruses.txt).
- countries_filepath: The path to a file containing country names, like [this one](bash/data/countries.txt).
- number_of_records: The number of records that will be produced in the dataset.
- allow_duplicates: Whether to allow duplicate (same citizen ID) records or not. Should have 0 as value if duplicates are not allowed. Else, they are allowed.

The path of the created dataset is by default: [bash/tests/inputFile.txt](bash/tests/inputFile.txt).

An example of running the script is:
```shell
$ ./testFile.sh data/viruses.txt data/countries.txt 10000 1
```

To compile the source code, run
```shell
$ cd ..
$ make
```

To run the executable, enter
```shell
$ bin/vaccineMonitor -b Bloom_Filter_size_in_Bytes -c Path_to_Dataset
```
where the parameters:
- -b Bloom_Filter_size_in_Bytes: Integer value indicating the number of bytes to allocate for the Bloom Filter.
- -c Path_to_Dataset: The absolute/relative path to the Dataset used for initially inserting data in the database.

After running the program, the user will get a prompt explaining the available options and queries available.

An example of running the program is:
```shell
$ bin/vaccineMonitor -b 100000 -c bash/tests/inputFile.txt
```

Furthemore, [Valgrind](https://valgrind.org/) can be used to check the memory management, like so:
```shell
$ valgrind bin/vaccineMonitor -b 100000 -c bash/tests/inputFile.txt
```