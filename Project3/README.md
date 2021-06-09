# Travel Monitor Client

This is the third out of three projects of the [Systems Programming](http://cgi.di.uoa.gr/~antoulas/k24/) course offered by the [Department of Informatics and Telecommunications](https://www.di.uoa.gr/en) of the [National and Kapodistrian University of Athens]((https://en.uoa.gr/)). It's purpose is to build a complete system that will be used to monitor travel requests of citizens (Records) from a country to another. The system uses many child (worker) processes in order to handle the input data and give answers to queries. The workers use a group of threads in order to parse the datasets. The IPC is achieved with the usage of Sockets.
<br> </br>


# Repository Structure

- [bin](bin): Contains the executables produced from compilation.
- [object](object): Contains object files (*.o) produced from compilation.
- [logiles](logfiles): Will contain the logfiles of the travelMonitorClient and the monitorServer once their execution has finished.
- [include](include): Contains all the header files for the project. It is divided in three sub-directories:
    - [data_structures](include/data_structures): Contains Header files for all the data structures used by the system.
    - [utils](include/utils): Contains Header files for various utility functions/structures, including the socket/multithreading variable initializations.
    - [ipc](include/ipc): Contains Header files of all the functions used for the communication of the travelMonitorClient and the monitorServer-children processes.
- [core](core): Contains the core source code of the project. It contains three sub-directories and two main source files:
    - [data_structures](core/data_structures): Contains the implementations of the data structures defined in the correspnding header files.
    - [utils](core/utils): Contains the implementations of all the utility functions.
    - [ipc](core/ipc): Contains the implementations of all the IPC-related functions.
    - [travelMonitorClient.cc](core/travelMonitorClient.cc): The main source file for the travelMonitorClient module.
    - [monitorServer.cc](core/monitorServer.cc): The main source file for the monitorServer module.
<br> </br>


# Implementation Details

## Pipeline

When executing the [travelMonitorClient](core/travelMonitorClient.cc) program, the following actions take place:

1. The Network Addresses (IP and port number) for each Monitor are initialized.
2. The [monitorServer](core/monitorServer.cc) processes get created with the help of the [fork()](https://man7.org/linux/man-pages/man2/fork.2.html) and [execvp()](https://linux.die.net/man/3/execvp) system calls.
3. Each [monitorServer](core/monitorServer.cc) establishes a connection with the parent ([travelMonitorClient](core/travelMonitorClient.cc)) process. If no countries were given in the argument list of the [monitorServer](core/monitorServer.cc), then it quits.
4. Each [monitorServer](core/monitorServer.cc) creates "num_threads" threads that are used to parse the countries directories specified in their arguments.
5. The [monitorServer](core/monitorServer.cc) processes parse the files of the specified countries in order to setup their data structures (Linked Lists, Skip Lists and Bloom Filters). After the parsing has been completed, pairs (Virus Name - Bloom Filter for that virus) are sent to the parent process ([travelMonitorClient](core/travelMonitorClient.cc)). The [travelMonitorClient](core/travelMonitorClient.cc), with the help of the [poll()](https://man7.org/linux/man-pages/man2/poll.2.html) system call identifies which [monitorServer](core/monitorServer.cc) processes have finished, in order to accept their data first, thus not wasting time waiting.
6. The [travelMonitorClient](core/travelMonitorClient.cc) process is now ready to accept queries and forward them to the child (Monitor) processes.

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

<br> </br>


# Usage

To compile the source code, run
```shell
$ make
```

To run the executable, enter
```shell
$ bin/travelMonitorClient bin/travelMonitorClient -m numMonitors
                                                  -b socketBufferSize
                                                  -c cyclicBufferSize
                                                  -s sizeOfBloom
                                                  -i input_dir
                                                  -t numThreads
```
where the parameters:
- -m numMonitors: Integer value that indicates how many child (Monitor) processes shall be created.
- -b socketBufferSize: Integer value denoting the maximum number of bytes that messages should have when being transfered through the sockets.
- -c cyclicBufferSize: Integer value denoting the maximum number of dataset files than can be present in a Cyclic Buffer from which the threads will consume.
- -s sizeOfBloom: Integer value indicating the number of bytes to allocate for the Bloom Filter.
- -i input_dir: The absolute/relative path to the root directory that contains the sub-directories with the countries.
- -t: Integer value denoting the numbe of threads to create per monitor process.

After running the program, the user will get a prompt explaining the available options and queries available.

An example of running the program is:
```shell
$ bin/travelMonitorClient -m 3 -b 420 -c 5 -s 10 -i dataset -t 4
```

Furthemore, [Valgrind](https://valgrind.org/) can be used to check the memory management, like so:
```shell
$ valgrind --trace-children=yes --leak-check=full bin/travelMonitorClient -m 3 -b 420 -c 5 -s 10 -i dataset -t 4
```