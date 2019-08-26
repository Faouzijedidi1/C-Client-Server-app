// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include<sys/utsname.h>
#include "rdwrn.h"

void get_name_id_ip(int socket)
{
    char name_id_ip[32];
    size_t k;
	//Getting the information from the server
    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) name_id_ip, k);


    printf(" Student Name  Student ID  Server IP Adress\n %s\n", name_id_ip);
    printf("Received: %zu bytes\n\n", k);
}
// Display the Utsname server Information
void displayUtsname(int socket)
	{
    struct utsname uts;
	//Check if the uts exists
    if (uname(&uts) == -1) {
	perror("uname error");
	exit(EXIT_FAILURE);
    }
        size_t n;
	//Getting the information from the server
        readn(socket, (unsigned char *) &n, sizeof(size_t));
        readn(socket, (unsigned char *) uts.nodename, n);
        readn(socket, (unsigned char *) uts.sysname, n);
        readn(socket, (unsigned char *) uts.release, n);
        readn(socket, (unsigned char *) uts.version, n);
        readn(socket, (unsigned char *) uts.machine, n);

//Print the details
    printf("Node name:    %s\n", uts.nodename);
    printf("System name:  %s\n", uts.sysname);
    printf("Release:      %s\n", uts.release);
    printf("Version:      %s\n", uts.version);
    printf("Machine:      %s\n", uts.machine);

	}

void get_random_array(int socket)
{

	size_t k;	
	char rand_num[32];
	//Getting the information from the server
	   	readn(socket, (unsigned char *) &k, sizeof(size_t));	  
	   	readn(socket, (unsigned char *) rand_num, k);

	  // print out details of received struct
	   	printf("Random array of 5 numbers are: %s\n", rand_num);
}

void get_file_list(int socket)
{
    size_t k;
	//Getting the information from the server
    readn(socket, (unsigned char *) &k, sizeof(size_t));
    char filelist[k];   
    readn(socket, (unsigned char *) filelist, k);

    printf("Received: %zu bytes\n\n", k);
    printf("\n---Files On Server -------------------\n");
    printf("%s", filelist);
    printf("\n--------------------------------------\n"); 

} // end get_file_list()
int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50001);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");

    // *** 
    // Menu COde 
        int choice;
    do
    {
	printf("*****************************************************************\n");
        printf("*                    Choose an Option:                          *\n");
	printf("*****************************************************************\n");
        printf("*    1. Display Student Details                                 *\n");
        printf("*    2. Display the random Array                                *\n");
        printf("*    3. Display the uname information                           *\n");
        printf("*    4. Display the files in server upload directory            *\n");
        printf("*    5. Exit                                                    *\n");
	printf("*****************************************************************\n\n");
        scanf("%d",&choice);
   
         switch (choice)
        {
          	case 1: {
        		// get a string from the server
		size_t payload_length = strlen("1")+1;

		  // send the original struct
		   	writen(sockfd, (unsigned char *) &payload_length, sizeof(size_t));	
		   	writen(sockfd, (unsigned char *) "1", payload_length);

  			get_name_id_ip(sockfd);}
           		break;
                case 2:{
			size_t payload_length = strlen("2")+1;

			  // send the original struct
			   	writen(sockfd, (unsigned char *) &payload_length, sizeof(size_t));	
			   	writen(sockfd, (unsigned char *) "2", payload_length);
		    	get_random_array(sockfd);}
                break;
                case 3: {
			size_t payload_length = strlen("3")+1;

			  // send the original struct
			   	writen(sockfd, (unsigned char *) &payload_length, sizeof(size_t));	
			   	writen(sockfd, (unsigned char *) "3", payload_length);


			displayUtsname(sockfd);}
                break;
               case 4: {
			size_t payload_length = strlen("4")+1;

			  // send the original struct
			   	writen(sockfd, (unsigned char *) &payload_length, sizeof(size_t));	
			   	writen(sockfd, (unsigned char *) "4", payload_length);


			get_file_list(sockfd);}
                break;
                case 5: 
          	  printf("Goodbye\n"); 
                break;
                default: printf("Wrong Choice. Enter again\n");
                    break;
        }    
  
    } while (choice != 5);



    // *** make sure sockets are cleaned up

    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()
// Part of the attempt to implement the enhancement
void get_file_from_server(char *fname, char *data)
{
	FILE *fp;
	int len = strlen(data);
	char strg[len];
	strcpy(strg, data);
	fp = fopen(fname, "w");
	fwrite(strg, 1, sizeof(strg), fp);
	fclose(fp);
}
//Part of the enhancement attempt
void get_fname(char *filea, int size)
{
	getchar();
	fgets(filea, size, stdin);
}
