// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <dirent.h>
#include<sys/utsname.h>
// thread function
void *client_handler(void *);
void send_hello(int);
void send_name_id_ip(int);
char *serverip();
void displayUname(int);
void send_random_array(int);
void send_file_list(int);


// you shouldn't need to change main() in the server except the port number
int main(void)
{
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
	printf("Waiting for a client to connect...\n");
	connfd =
	    accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
	    (&sniffer_thread, NULL, client_handler,
	     (void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    	int connfd = *(int *) socket_desc;
	// The request length
 	size_t payload_length;
	//Declare the variable to save the response from the client 
  	char req[2];
		while(1)
		{
		//Receiving the option choice from the client
		readn(connfd, (unsigned char *) &payload_length, sizeof(size_t));
		readn(connfd, (unsigned char *) req, payload_length);

		if(req[0] == '1')
		{	//calling the name, id and ip address function
      			  send_name_id_ip(connfd);	 

		}

		if(req[0] == '2')
		{
		//printf("string...2");
		//start timer to not get the same random numbers
		srand(time(0));
		//Calling the random array function
		send_random_array(connfd);
	
		}

		if(req[0] == '3')
		{	//calling the uname function
			displayUname(connfd);
		}
		if(req[0] == '4')
		{	//calling the file list function
			send_file_list(connfd);
		}
		}




    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()
// The server ip function to retreive the ip address of the server
char *serverip()
{
	char serverIpAddress[256];
	struct hostent *host_address;
	int hostname;
	char *temp_addr;

	hostname = gethostname(serverIpAddress, sizeof(serverIpAddress));
	host_address = gethostbyname(serverIpAddress);
	temp_addr = inet_ntoa(*((struct in_addr*) host_address->h_addr_list[0]));
	return temp_addr;
}
// the function that sends the name, id and ip address
void send_name_id_ip(int socket)
{
	char *ip_addr;
	ip_addr = serverip();
	char student_name[] = "Faouzi Jedidi ";
	char student_id[] = " S1719017    ";
	size_t n = strlen(student_name) + strlen(student_id) + strlen(ip_addr)+ 1;
	char *name_id_ip = malloc(n);
	strcpy (name_id_ip, student_name);
	strcat (name_id_ip, student_id);
	strcat (name_id_ip, ip_addr); 

	    writen(socket, (unsigned char *) &n, sizeof(size_t));	
	    writen(socket, (unsigned char *) name_id_ip, n);	

}
// A retrieved function to implement the itoa that converts the 
  char* itoa(int value, char* result, int base) 
{
      // check that the base if valid
      if (base < 2 || base > 36) 
{ 
*result = '\0'; 
return result; 
}

      char* ptr = result, *ptr1 = result, tmp_char;
      int tmp_value;

      do {
  tmp_value = value;
  value /= base;
  *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while ( value );

      // Apply negative sign
      if (tmp_value < 0) *ptr++ = '-';
      *ptr-- = '\0';
      while(ptr1 < ptr)
{
  tmp_char = *ptr;
  *ptr--= *ptr1;
  *ptr1++ = tmp_char;
      }
      return result;
  }
//The random array function to generate and send random array 
void send_random_array(int socket)
{	

	char rand_array[5];
	int i;
	char num_array[40] = "";

	for(i=0; i < 5;i++)
	{
	int num = (rand() % 1000 - 0 + 1 ) + 0;
	itoa(num, rand_array, 10);
	strcat (num_array,rand_array);
	if(i < 5-1){ strcat(num_array, "   "); }
	}

	//printf("%s", num_array);
	size_t payload_length = strlen(num_array)+1;

	  // send the original struct
	   	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
	   	writen(socket, (unsigned char *) num_array, payload_length);
}
// retrieve and send the uts information
void displayUname(int socket){
    struct utsname uts;
    errno = 0;

    if (uname(&uts) != 0) {
        perror("uname error");
        exit(EXIT_FAILURE);
        }
    size_t k;
    
    writen(socket, (unsigned char *) &k, sizeof(size_t));   
    writen(socket, (unsigned char *) uts.nodename,k); 
    writen(socket, (unsigned char *) uts.sysname,k); 
    writen(socket, (unsigned char *) uts.release,k); 
    writen(socket, (unsigned char *) uts.version,k);
    writen(socket, (unsigned char *) uts.machine,k);
}
//the function to get and send the list of files in upload directory 
void send_file_list(int socketNumber)
{
    DIR *mydir;
    if ((mydir = opendir("upload")) == NULL) {
    perror("error");
    exit(EXIT_FAILURE);
    }

    struct dirent *entry = NULL;


    char filelist[255];

    //returns NULL when dir contents all processed
    while ((entry = readdir(mydir)) != NULL)
    {
    strcat(strcat(filelist, entry->d_name),"\n");
    }

    closedir(mydir);

    size_t n = strlen(filelist);

    writen(socketNumber, (unsigned char *) &n, sizeof(size_t)); 
    writen(socketNumber, (unsigned char *) filelist, n);

    printf("Sent file list of size %zu bytes\n",n);

}//end send_file_list()
// a non finished attempt to implement the uploading if the files to the client excutable directory 
void upload_file(int socket, char *filename, FILE *myFile){

    struct dirent **namelist;
    char myFiles[255] = "";
    size_t payload_length;

    char ch;

    int i = 0;
    
    char buffer[1024];

    int n;

    n = scandir("upload", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
            if (strcmp(namelist[n]->d_name, filename))
            {
                myFile = fopen(filename, "r"); // read mode
 
                if (myFile == NULL)
                {
                    perror("Error while opening the file.\n");
                    exit(EXIT_FAILURE);
                }else{

                    while((ch = fgetc(myFile)) != EOF)
                    {
                        buffer[i] = ch;
                        i++;
                    }

                    size_t payload_length = strlen(buffer) + 1;

                    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
                    writen(socket, (unsigned char *) buffer, payload_length);
                }

                fclose(myFile);
                
            }else{

                payload_length = strlen("0") + 1;
                writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
                writen(socket, (unsigned char *) "0", payload_length);
                break;
            }
            
            if((n - 2) == 0) n = 0;
            free(namelist[n]);
        }
            free(namelist);

    }

}



