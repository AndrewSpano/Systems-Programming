# Travel Monitor

This is the second out of three projects of the [Systems Programming](http://cgi.di.uoa.gr/~antoulas/k24/) course offered by the [Department of Informatics and Telecommunications](https://www.di.uoa.gr/en) of the [National and Kapodistrian University of Athens]((https://en.uoa.gr/)). It's purpose is to build a complete system that will be used to monitor travel requests of citizens (Records) from a country to another. The system uses many child (worker) processes in order to handle the input data and give answerz to queries. The IPC is achieved with the usage of Named Pipes.
<br> </br>


# Repository Structure

- [bin](bin): Contains the executables produced from compilation.
- [object](object): Contains object files (*.o) produced from compilation.
- [include](include): Contains all the header files for the project. It is divided in four sub-directories:
    - [data_structures](include/data_structures): Contains Header files for all the data structures used by the system.
    - [utils](include/utils): Contains Header files for various utility functions/structures.
    - [ipc](include/ipc): Contains Header files of all the functions used for the communication of the travelMonitor and the Monitor-child processes.
    - [signal_handlers](include/singal_handlers): Contains Header files for the functions used to handle signals for the travelMonitor and Monitor modules.
- [core](core): Contains the core source code of the project. It contains two four-directories and two main source files:
    - [data_structures](core/data_structures): Contains the implementations of the data structures defined in the correspnding header files.
    - [utils](core/utils): Contains the implementations of all the utility functions.
    - [ipc](core/ipc): Contains the implementations of all the IPC-related functions.
    - [signal_handlers](core/signal_handlers): Contains the implementations of all the signal-related functions. 
    - [travelMonitor.cc](core/travelMonitor.cc): The main source file for the travelMonitor module.
    - [Monitor.cc](core/Monitor.cc): The main source file for the Monitor module.
- [bash](bash): Contains the [bash script](bash/create_infiles.sh) used to create test datasets.
<br> </br>


# Implementation Details

## Pipeline

When executing the [travelMonitor](core/travelMonitor.cc) program, the following actions take place:

1. A pair of named pipes (for every [Monitor](core/Monitor.cc)) get created and placed in the [pipes](pipes) directory. The pair consists of input-output named pipes.
2. The [Monitor](core/Monitor.cc) processes get created with the help of the [fork()](https://man7.org/linux/man-pages/man2/fork.2.html) and [execvp()](https://linux.die.net/man/3/execvp) system calls.
3. The input of the [travelMonitor](core/travelMonitor.cc) process (Bloom Filter size, Buffer size and root directory) is sent to each [Monitor](core/Monitor.cc) process using the Named Pipes.
4. The [travelMonitor](core/travelMonitor.cc) process scans the root directory and assigns countries to each [Monitor](core/Monitor.cc).
5. The [Monitor](core/Monitor.cc) processes parse the files of the specified countries in order to setup their data structures (Linked Lists, Skip Lists and Bloom Filters). After the parsing has been completed, pairs (Virus Name - Bloom Filter for that virus) are sent to the parent process ([travelMonitor](core/travelMonitor.cc)). The [travelMonitor](core/travelMonitor.cc), with the help of the [poll()](https://man7.org/linux/man-pages/man2/poll.2.html) system call identifies which [Monitor](core/Monitor.cc) processes have finished, in order to accept their data first, thus not wasting time waiting.
6. The [travelMonitor](core/travelMonitor.cc) process is now ready to accept queries and forward them to the child (Monitor) processes.

## IPC

The [Inter-Process Communication](https://en.wikipedia.org/wiki/Inter-process_communication) is achieved by using the following functions

- ipc::_send_numeric()
- ipc::_receive_numeric()

- ipc::_send_message()
- ipc::_receive_message()

defined in the file [ipc.hpp](include/ipc/ipc.hpp) and implemented in the file [ipc.cc](core/ipc/ipc.cc). I believe their purpose is self-explanatory.

The protocol used for communication uses message IDs defined in [ipc.hpp](include/ipc/ipc.hpp). It goes like this:

- Sending-Receiving a Numeric value
    1. The sender splits the 8 bytes of the numeric value into chunks of maximum size "Buffer Size" bytes.
    2. It starts sending of the first chunk. Then, it waits to receive the message ID "ACK" from the receiver in order to proceed and send the next chunk.
    3. Both processes know that the size of a numeric value is 8 bytes. Thus, once the 8 bytes have been sent and received, the communication finishes. Note that the sender has to receive an "ACK" for the last chunk of bytes that it sent.

- Sending-Receiving a Message (array of characters)
    1. The message ID is sent to the receiver. An "ACK" response is then awaited as confirmation of receival.
    2. The length of the message (in bytes) is sent to the receiver, using the ipc::_send_numeric() function.
    3. Then, the original message is split into chunks of maximum size "Buffer Size" bytes. Each chunk is sent to the receiver, which replies with an "ACK" message in order to confirm the receival of the chunk. After "ACK" has been received by the sender, the next chunk is sent. And so on until the whole message has been sent and the final "ACK" response has been received by the sender.

## Data Structures

The data structures are the same that were used in Project 1, slightly modiefied to fit the requirement of this project. Also note that in this project, there is a separate "Menu" for every module. They are defined in [indices.hpp](include/data_structures/indices.hpp), and they are used to group together and carry arround all the different structures and functionalities that these modules need to have access to.

## Signal Handling

Signal Handlers for each module have been implemented in the [core/signal_handlers](core/signal_handlers) directory. The signals are blocked when queries are being executed, and unblocked (thus received and handled) when the query finishes. Their handling is performed on the fly. Flags could have been used instead, but there is no need for them.
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