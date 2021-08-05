# Systems Programming

Assignments implemented for the K24 Systems Programming course offered at the Department of Informatics and Telecommunications, University of Athens, Greece. <br> </br>

# [Assignment 1](Project1)

The goal of this assignment is to implement a System that will Monitor vaccinations for different viruses. The system must respond quickly to a variety of queries. This is accomplished with the usage of data structures like Bloom Filters, Hash Tables, Skip Lists, etc. <br> </br>

# [Assignment 2](Project2)

The goal of this assignment is to implement a System that will Monitor travel requests from various countries. The system must decide quickly whether a citizen is allowed to travel from one country to another, by checking if he has been vaccinated for a specific virus. The idea behind this project is to divide the dataset in smaller chunks and assign each chunk to a chuld process. The main process will then forward queries to the child process with the help of Named Pipes. Also, signal handlers are used in order to avoid any errors or re-build a child process in case it terminates unexpectedly. <br> </br>


# [Assignment 3](Project3)

The goal of this assignment is the same as the previous one, with the only difference that now the IPC is achieved with sockets, and that every Monitor has a number of threads used to build quicker the initial data structures. <br> </br>
