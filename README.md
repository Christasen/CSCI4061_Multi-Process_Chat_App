/* CSci4061 F2018 Assignment 2
* section: one digit number
* date: 11/09/2018
* name: Guangyu Yan, Huilin Zhang, Ziqian Qiu (for partner)
* id: yanxx630, zhan5992, qiuxx283/


## The purpose of your program
The purpose of our main program is to gain experiences with the following three concepts.

a). The use of processes to decompose an apllication and to provide isolation.

b). The use of interprocess communication (or IPC) to coordinate and communicate among the processses.

c). The use of "polling" to implement asynchrony in the absence of threads.



## Who did what on the program
We did all the algorithms and most of our coding and debugging work during our meeting.

* Guangyu Yan

    *Guangyu finished the whole program design and contributes ideas of the whole framework algorithms design.

* Ziqian Qiu

    *Ziqian finished the user program and contributes ideas of the whole framework algotithms design.


* Huilin Zhang

    *Huilin finished the command handlers in the server program and conributes the ideas of the whole frawork algorithms design.


## How to compile the program
There would be a Makeile for the program in the program repo.

a) Open terminal, enter the repo of the program, run *make

b) ./server: This is the way to create a server

c) ./client <clientname>: This is the way to add a client by giving a <clientname>. The client name would be a user defined variable.




## How to use the program from the shell (syntax)
Creating Server:
1. ./server : This is the way to create a server

### Creating Clients
2. ./client <clientname> : This is the way to add a client by giving a <clientname>. The client name would be a user defined variable.


### For the SERVER Process
1. \list : List all of the users currently connected to the server. Print ’<no users>’ if there are no users currently.

2. \kick <username> : Kick the specified user off the chat session.

3. \exit : Terminate all user sessions and close the chat server as well.

4. <any-other-text> : Broadcast this text to all of the user processes with prefix, “admin:”. Do nothing, if no users
are connected to the chat. The users will print out the message. admin: <any-other-text>


### For the USER Process
1. \list : Same as in the SERVER process. Output should be printed in the user’s process terminal window.

2. \exit : Disconnect this user. Terminate the USER process and remove them from the chat session.

3. \p2p <username><message> : Send a personal message (<message>) to the user specified in <username>.
Print error if the user is invalid.

4. <any-other-text> : Same as in the SERVER process but without a prefix. Broadcast this text to all user pro-
cesses.


## What exactly your program does

This program is a simple local multi-party chat application using a multi-process architecture.
The chat processes will all run on a single machine locally and there will be a central chat server who can handles
all the management of the chat and waits for users o connect to the server. Also, the chat server will support both
peer to peer chatting and group chatting. Meanwhie, there are will only have one chatting group and this chatting group will
contains all users that are connected to the server.



## Any explicit assumptions you have made

1. The user would not type anything which seems like stupid.

2. User will follow the command syntax we mentioned above.

3. User will follow the instructions we menthioned above.

4. User will not type in bad commands.

5. We assume that we dont have to write the show error message function.

6. There are no printing message for the pipe numbers when you add a new user.

7. The program will dectect the full room assumption first and then the program will
detect the same target condition.




## Your strategies for error handling

1. If the program encounters and error, more specifically speanking, if an invalid user name is
supplied in the \p2p command, a useful error message will be printed to the screen.

2. If any error prevents your program from functioning normally,then it should exit after printing the error message. (The use of the perror() function for printing error messages isencouraged.) Upon executing the \exit command on the SERVER, the main SERVER process and its child processes (if exist) must exit properly, cleaning-up all of the users, waiting for all child processes and freeing up any used resources.

3. If the room of the pipe is full, we print an error message.

4. If the the user type in a user name that is already exsiting, we print an error message.
