/* CSci4061 F2018 Assignment 2
* section: one digit number
* date: 11/09/2018
* name: Guangyu Yan, Huilin Zhang, Ziqian Qiu (for partner)
* id: yanxx630, zhan5992, qiuxx283/

*/



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "comm.h"
#include "util.h"

/* -----------Functions that implement server functionality -------------------------*/

/*
 * Returns the empty slot on success, or -1 on failure
 */
int find_empty_slot(USER * user_list) {
	// iterate through the user_list and check m_status to see if any slot is EMPTY
	// return the index of the empty slot
  int i = 0;
	for(i=0;i<MAX_USER;i++) {
    if(user_list[i].m_status == SLOT_EMPTY) {
		return i;
		}
	}
	return -1;
}

/*
 * list the existing users on the server shell
 */
int list_users(int idx, USER * user_list)
{
	// iterate through the user list
	// if you find any slot which is not empty, print that m_user_id
	// if every slot is empty, print "<no users>""
	// If the function is called by the server (that is, idx is -1), then printf the list
	// If the function is called by the user, then send the list to the user using write() and passing m_fd_to_user
	// return 0 on success
	int i, flag = 0;
	char buf[MAX_MSG] = {}, *s = NULL;

	/* construct a list of user names */
	s = buf;
	strncpy(s, "---connecetd user list---\n", strlen("---connecetd user list---\n"));
	s += strlen("---connecetd user list---\n");
	for (i = 0; i < MAX_USER; i++) {
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		flag = 1;
		strncpy(s, user_list[i].m_user_id, strlen(user_list[i].m_user_id));
		s = s + strlen(user_list[i].m_user_id);
		strncpy(s, "\n", 1);
		s++;
	}
	if (flag == 0) {
		strcpy(buf, "<no users>\n");
	} else {
		s--;
		strncpy(s, "\0", 1);
	}

	if(idx < 0) {
		printf("%s", buf);
		printf("\n");
	} else {
		/* write to the given pipe fd */
		if (write(user_list[idx].m_fd_to_user, buf, strlen(buf) + 1) < 0)
			perror("writing to server shell");
	}

	return 0;
}

/*
 * add a new user
 */
int add_user(int idx, USER * user_list, int pid, char * user_id, int pipe_to_child, int pipe_to_parent)
{
	// populate the user_list structure with the arguments passed to this function
	// return the index of user added
    user_list[idx].m_pid = pid;
    strcpy(user_list[idx].m_user_id, user_id);
    user_list[idx].m_fd_to_user = pipe_to_child;
    user_list[idx].m_fd_to_server = pipe_to_parent;
    user_list[idx].m_status = SLOT_FULL;
	return idx;
}

/*
 * Kill a user
 */
void kill_user(int idx, USER * user_list) {
	// kill a user (specified by idx) by using the systemcall kill()
	// then call waitpid on the user
    //exec("kill", "-9", user_list[idx].m_pid);
    kill(user_list[idx].m_pid, SIGKILL);
    waitpid(user_list[idx].m_pid, NULL, 0);
}

/*
 * Perform cleanup actions after the used has been killed
 */
void cleanup_user(int idx, USER * user_list)
{
	// m_pid should be set back to -1
	// m_user_id should be set to zero, using memset()
	// close all the fd
	// set the value of all fd back to -1
	// set the status back to empty
    user_list[idx].m_pid = -1;
    memset(user_list[idx].m_user_id, 0, sizeof user_list[idx].m_user_id);
    close(user_list[idx].m_fd_to_server);
    close(user_list[idx].m_fd_to_user);
    user_list[idx].m_fd_to_user = -1;
    user_list[idx].m_fd_to_server = -1;
    user_list[idx].m_status = SLOT_EMPTY;
}

/*
 * Kills the user and performs cleanup
 */
void kick_user(int idx, USER * user_list) {
	// should kill_user()
	// then perform cleanup_user()
    kill_user(idx, user_list);
    cleanup_user(idx, user_list);
}

/*
 * broadcast message to all users
 */


int broadcast_msg(USER * user_list, char *buf, char *sender)
{
	//iterate over the user_list and if a slot is full, and the user is not the sender itself,
	//then send the message to that user
	//return zero on success
    int i = 0;
    for(; i < MAX_USER; i++){
        if(user_list[i].m_status == SLOT_FULL && strcmp(user_list[i].m_user_id, sender) != 0){
            write(user_list[i].m_fd_to_user, buf, strlen(buf));
        }
    }
	return 0;
}

/*
 * Cleanup user chat boxes
 */
void cleanup_users(USER * user_list)
{
	// go over the user list and check for any empty slots
	// call cleanup user for each of those users.
    for(int i = 0; i < MAX_USER; i++){
        if(user_list[i].m_status != SLOT_EMPTY){
            // kill_user(i, user_list);
            cleanup_user(i, user_list);
        }
    }
}

/*
 * find user index for given user name
 */
int find_user_index(USER * user_list, char * user_id)
{
	// go over the  user list to return the index of the user which matches the argument user_id
	// return -1 if not found
	int i, user_idx = -1;

	if (user_id == NULL) {
		fprintf(stderr, "NULL name passed.\n");
		return user_idx;
	}
	for (i=0;i<MAX_USER;i++) {
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		if (strcmp(user_list[i].m_user_id, user_id) == 0) {
			return i;
		}
	}

	return -1;
}

/*
 * given a command's input buffer, extract name
 */
int extract_name(char * buf, char * user_name)
{
	char inbuf[MAX_MSG];
    char * tokens[16];
    strcpy(inbuf, buf);

    int token_cnt = parse_line(inbuf, tokens, " ");

    if(token_cnt >= 2) {
        strcpy(user_name, tokens[1]);
        return 0;
    }

    return -1;
}

int extract_text(char *buf, char * text)
{
    char inbuf[MAX_MSG];
    char * tokens[16];
    char * s = NULL;
    strcpy(inbuf, buf);

    int token_cnt = parse_line(inbuf, tokens, " ");

    if(token_cnt >= 3) {
        //Find " "
        s = strchr(buf, ' ');
        s = strchr(s+1, ' ');

        strcpy(text, s+1);
        return 0;
    }

    return -1;
}

/*
 * send personal message
 */
void send_p2p_msg(int idx, USER * user_list, char *buf)
{

	// get the target user by name using extract_name() function
	// find the user id using find_user_index()
	// if user not found, write back to the original user "User not found", using the write()function on pipes.
	// if the user is found then write the message that the user wants to send to that user.

    char recv_name[MAX_MSG];
    char msg[MAX_MSG];
    memset(recv_name, '\0',MAX_MSG);
    memset(msg, '\0',MAX_MSG);
    extract_name(buf, recv_name);
    int idr = find_user_index(user_list, recv_name);
    if (idr == -1){
        strcpy(buf, "User not found");
        write(user_list[idx].m_fd_to_user, buf, strlen(buf));
    }
    else{
      char  text[MAX_MSG];
      memset(text, '\0',MAX_MSG);
      extract_text(buf, text);
      printf("msg : %s", text);
      sprintf(msg, "%s: %s", user_list[idx].m_user_id, text);
    //  parse_line(buf, tokens, " ");
      write(user_list[idr].m_fd_to_user, msg, strlen(msg));
      printf("\n");
      printf("msg: %s", msg);
      print_prompt("\nadmin");

    }
}


//takes in the filename of the file being executed, and prints an error message stating the commands and their usage
void show_error_message(char *filename)
{
    printf("%s\n", filename);
}


/*
 * Populates the user list initially
 */
void init_user_list(USER * user_list) {

	//iterate over the MAX_USER
	//memset() all m_user_id to zero
	//set all fd to -1
	//set the status to be EMPTY
	int i=0;
	for(i=0;i<MAX_USER;i++) {
		user_list[i].m_pid = -1;
		memset(user_list[i].m_user_id, '\0', MAX_USER_ID);
		user_list[i].m_fd_to_user = -1;
		user_list[i].m_fd_to_server = -1;
		user_list[i].m_status = SLOT_EMPTY;
	}
}

void seg()
{
  char *n=NULL;
  *n=1;

}

/* ---------------------End of the functions that implementServer functionality -----------------*/


/* ---------------------Start of the Main function ----------------------------------------------*/
int main(int argc, char * argv[])
{
	int nbytes;
	setup_connection("BestGroup"); // Specifies the connection point as argument.

	USER user_list[MAX_USER];
	init_user_list(user_list);   // Initialize user list

	char buf[MAX_MSG];
	fcntl(0, F_SETFL, fcntl(0, F_GETFL)| O_NONBLOCK);
	print_prompt("admin");

    int quit_flag = 0;
	//
	while(1) {
		/* ------------------------YOUR CODE FOR MAIN--------------------------------*/
		usleep(1000);
		// Handling a new connection using get_connection
		int pipe_SERVER_reading_from_child[2];
		int pipe_SERVER_writing_to_child[2];
		char user_id[MAX_USER_ID];
    int pipe_child_reading_from_user[2];
    int pipe_child_writing_to_user[2];



        if(get_connection(user_id, pipe_child_writing_to_user, pipe_child_reading_from_user) != -1){
		    // Check max user and same user id
            int idx = find_empty_slot(user_list);
            int flag = find_user_index(user_list, user_id);

            /*
             * idx != -1 && flag != -1
             */

            if(idx == -1){
              write(pipe_child_writing_to_user[1],"No more room for a new user",MAX_MSG);
              printf("No more room for a new user\n");
              print_prompt("admin");
            } else if (flag > -1) {
              write(pipe_child_writing_to_user[1],"User id already taken", MAX_MSG);
              printf("User id: %s already taken\n", user_id);
              print_prompt("admin");

            } else {

            printf("\nA new user: %s connected. Slot: %d\n", user_id, idx);
            print_prompt("admin");
            // printf("create username: %s\n", user_id);
            pipe(pipe_SERVER_writing_to_child);
            pipe(pipe_SERVER_reading_from_child);
            int pid = fork();

            // Child process: poli users and SERVER
            if(pid == 0){
                // close SERVER pipe
                close(pipe_SERVER_reading_from_child[0]);    //close reading pipe
                close(pipe_SERVER_writing_to_child[1]);
                // clode child pipe
                close(pipe_child_writing_to_user[0]);
                close(pipe_child_reading_from_user[1]);

                fcntl(pipe_child_reading_from_user[0],F_SETFL,fcntl(pipe_child_reading_from_user[0],F_GETFL)|O_NONBLOCK);

                fcntl(pipe_SERVER_writing_to_child[0],F_SETFL,fcntl(pipe_SERVER_writing_to_child[0],F_GETFL)|O_NONBLOCK);

                while(1){
                    memset(buf, '\0', MAX_MSG);
                    if (read(pipe_child_reading_from_user[0], buf, MAX_MSG) > 0){
                        if (write(pipe_SERVER_reading_from_child[1], buf, strlen(buf)) == -1){
                            perror("Child to Server write error");
                        }
                        // printf("Child: %s\n", buf);

                    }
                    memset(buf, '\0', MAX_MSG);
                    if (read(pipe_SERVER_writing_to_child[0], buf, MAX_MSG) > 0){
                        if (write(pipe_child_writing_to_user[1], buf, strlen(buf)) == -1){
                            perror("Child to User write error");
                        }
                    }
                    usleep(1000);
                }
            }
		    // Server process: Add a new user information into an empty slot
            else{
                // printf("parent\n");
                close(pipe_child_reading_from_user[0]);
                close(pipe_child_writing_to_user[1]);
                close(pipe_SERVER_reading_from_child[1]);
                close(pipe_SERVER_writing_to_child[0]);
                fcntl(pipe_SERVER_reading_from_child[0], F_SETFL, O_NONBLOCK);
                add_user(idx, user_list, pid, user_id, pipe_SERVER_writing_to_child[1], pipe_SERVER_reading_from_child[0]);

            }
        }
        // poll child processes and handle user commands
		    // Poll stdin (input from the terminal) and handle admnistrative command
    }// end connection
        char msg[MAX_MSG];
        for (int i = 0; i < MAX_USER; i++){
            if(user_list[i].m_status == SLOT_FULL){
                memset(buf, '\0', MAX_MSG);
                usleep(1000);

                fcntl(user_list[i].m_fd_to_server,F_SETFL|O_NONBLOCK);

                if(read(user_list[i].m_fd_to_server, buf, MAX_MSG) >0){
                    // printf("Server recv: %s\n", buf);
                    buf[strlen(buf)-1] == '\0';
                    int _type = get_command_type(buf);
                    switch (_type){
                        case LIST:
                            list_users(i, user_list);
                            break;
                        case P2P:
                            send_p2p_msg(i, user_list, buf);
                            break;
                        case EXIT:
                        // printf("in exit \n" );
                            kick_user(i,user_list);
                            // write(user_list[i].m_fd_to_user, "exit", sizeof("exit"));
                            break;
                        case BROADCAST:
                            sprintf(msg, "%s: %s", user_list[i].m_user_id, buf);
                            //print_prompt("admin");
                            //printf("msg: %s", msg);
                            broadcast_msg(user_list, msg, user_list[i].m_user_id);
                            break;
                        case SEG :
                            cleanup_user(i,user_list);
                            break;

                    }//swich
                  }//if
            }// out if
        }//for
        usleep(1000);
        fcntl(0,F_SETFL,fcntl(0,F_GETFL)|O_NONBLOCK);
        char admin_command[MAX_MSG];
        memset(admin_command, '\0', MAX_MSG);
        if(read(0, admin_command, MAX_MSG) > 0){
            //execute admin_command
            admin_command[strlen(admin_command)-1] = '\0';
            // fflush(stdin);
            char user_id[MAX_MSG];
            // char start_ch[MAX_MSG] = "\\";
            int _type = get_command_type(admin_command);
            switch (_type){
                case LIST:
                    list_users(-1, user_list);
                    break;
                case KICK:
                    extract_name(admin_command, user_id);
                    int id_x = find_user_index(user_list, user_id);
                    if(id_x == -1){
                      printf("User not found\n");
                    } else {
                      sprintf(msg, "admin-notice: kick %s", user_list[id_x].m_user_id);
                      kick_user(id_x, user_list);
                      cleanup_user(id_x,user_list);
                      broadcast_msg(user_list, msg, "");
                    }
                    break;
                case EXIT:
                    broadcast_msg(user_list, "The server process seems dead.", "");
                    for(int i=0; i < MAX_USER;i++){
                     if (user_list[i].m_status == SLOT_FULL){
                       kill_user(i,user_list);
                     }
                    }
                    cleanup_users(user_list);
                    exit(-1);
                    quit_flag = 1;
                    break;
                case BROADCAST:
                    sprintf(msg, "Admin >> %s", admin_command);
                    broadcast_msg(user_list, msg, "");
                    break;

            } //swich
            print_prompt("admin");
          } // if

        if(quit_flag) break;
		/* ------------------------YOUR CODE FOR MAIN--------------------------------*/
	//while
}
}

/* --------------------End of the main function ----------------------------------------*/
