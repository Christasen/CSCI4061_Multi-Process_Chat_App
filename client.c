/* CSci4061 F2018 Assignment 2
* section: one digit number
* date: 11/09/2018
* name: Guangyu Yan, Huilin Zhang, Ziqian Qiu (for partner)
* id: yanxx630, zhan5992, qiuxx283/

*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "util.h"
#include "comm.h"

/* -------------------------Main function for the client ----------------------*/
void main(int argc, char * argv[]) {
	int tmp;
  char buf[MAX_MSG];
	int pipe_user_reading_from_server[2];
	int pipe_user_writing_to_server[2];

	// You will need to get user name as a parameter, argv[1].
	if(tmp = connect_to_server("BestGroup", argv[1], pipe_user_reading_from_server, pipe_user_writing_to_server) == -1) {
		exit(-1);
	}



    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
		fcntl(pipe_user_reading_from_server[0],F_SETFL,fcntl(pipe_user_reading_from_server[0],F_GETFL)|O_NONBLOCK);
		close(pipe_user_writing_to_server[0]);
		close(pipe_user_reading_from_server[1]);


	/* -------------- YOUR CODE STARTS HERE -----------------------------------*/
		memset(buf, '\0', MAX_MSG);
		if(read(pipe_user_reading_from_server[0],buf,MAX_MSG)>0)	{
		if(strcmp(buf,"No more room for a new user")){
			exit(-1);
		}
		else if(strcmp(buf,"User id already taken")){
			exit(-1);
		}
	}
		print_prompt(argv[1]);
		while(1){
	    // poll pipe retrieved and print it to sdiout
        memset(buf, '\0', MAX_MSG);
				usleep(1000);
				int result = read(pipe_user_reading_from_server[0], buf, MAX_MSG);
        if( result > -1){
					if(result >0){
            // printf("%s\n", buf);
						// printf("\n%s\n", buf);
						// if(strcmp(buf,"No more room for a new user")){
						// 	exit(-1);
						// }
						// else if(strcmp(buf,"User id already taken")){
						// 	exit(-1);
						// }
						printf("\n%s\n", buf);
						print_prompt(argv[1]);


            //executed
					}
            // if(strcmp(buf, "exit") == 0)
            //     break;
						if(result == 0) {
						//printf("\nenter it\n");
						close(pipe_user_writing_to_server[1]);
						close(pipe_user_reading_from_server[0]);
						exit(-1);
					}

        }

	    // Poll stdin (input from the terminal) and send it to server (child process) via pipe
      //  memset(buf, 0, sizeof buf);
				int bytes = read(0, buf, MAX_MSG);
        if(bytes > -1){

							if(bytes >0){

							buf[strlen(buf)-1]='\0';
							write(pipe_user_writing_to_server[1], buf, bytes);
							}

							int type = get_command_type(buf);

							if(type==SEG)
							{
								close(pipe_user_writing_to_server[1]);
								close(pipe_user_reading_from_server[0]);
								char *n=NULL;
								*n=1;

							}
							else if (type == EXIT){
								close(pipe_user_reading_from_server[0]);
								close(pipe_user_writing_to_server[1]);
								exit(-1);
							}
							print_prompt(argv[1]);

            // printf("command: %s\n", buf);
            // if(write(pipe_user_writing_to_server[1], buf, sizeof buf) == 1){
            //     perror("send to server error");
          }
        }

	/* -------------- YOUR CODE ENDS HERE -----------------------------------*/
}

/*--------------------------End of main for the client --------------------------*/
