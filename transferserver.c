#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <stdbool.h>

#define BUFSIZE 1033

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferserver [options]\n"                           \
    "options:\n"                                             \
    "  -f                  Filename (Default: 6200.txt)\n" \
    "  -h                  Show this help message\n"         \
    "  -p                  Port (Default: 12041)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"filename", required_argument, NULL, 'f'},
    {"help", no_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}};

int main(int argc, char **argv)
{
    int option_char;
    int portno = 12041;             /* port to listen on */
    char *filename = "6200.txt"; /* file to transfer */

    setbuf(stdout, NULL); // disable buffering

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:hf:x", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        case 'f': // listen-port
            filename = optarg;
            break;
        }
    }


    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    
    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    /******************** Create the server socket ***************************/
    int echoServer_socket = socket(AF_INET, SOCK_STREAM, 0);
    // error checking
    if (echoServer_socket < 0)
    {
      fprintf(stderr, "ERROR creating echoServer socket\n");
      exit(1);
    }

    /************************ Setup A Socket Host ***************************/
    int socket_reuse = 1;
    setsockopt(echoServer_socket, SOL_SOCKET, SO_REUSEADDR, 
               &socket_reuse, sizeof(socket_reuse));

    /******************* Specify Socket Address *****************************/
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));// Set address to Zeros
    server_address.sin_family      = AF_INET;          // Internet Protocol (AF_INET)
    server_address.sin_port        = htons(portno);    // Address port (16 bits)
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);// IP Address (32 bits)
    
    /******************* Bind Socket to IP and Port *************************/
    int bind_error = 0;
    bind_error = bind(echoServer_socket,
                     (struct sockaddr *) &server_address,
                      sizeof(server_address));
    // error checking
    if (bind_error < 0)
    {
      fprintf(stderr, "ERROR binding to incoming connections\n");
      exit(1);
    }

    int maxnpending = 7;  // maximum number of waiting clients.
    int listen_error = 0;
    listen(echoServer_socket, maxnpending);
    // error checking
    if (listen_error < 0)
    {
      fprintf(stderr, "ERROR opening a socket for listening\n");
      exit(1);
    }

    /******************* Accept & Connect to a New Client ********************/
    while (true)
    {
      char file_buffer[BUFSIZE];         // buffer to store read file contents
      int echoClient_socket;             // client socket file descriptor
      struct sockaddr_in client_addr;    // client address
      socklen_t client_addr_length;      //client address

      char *input_file_dir;   // Directory to read input file from
      FILE *file;             // File pointer
      size_t block_size;      // file block size

      client_addr_length = sizeof(echoClient_socket);
      echoClient_socket = accept(echoServer_socket, 
                                (struct sockaddr*)&client_addr, 
                                 &client_addr_length);
      // Error Checking
      if (echoClient_socket < 0)
      {
        fprintf(stderr, "ERROR to accept new client.\n");
        exit(1);
      }

      // Set input File Directory
      input_file_dir = getcwd(file_buffer, BUFSIZE);
      strcat(strcat(input_file_dir, "/"), filename);

      // Open File Location
      file = fopen(input_file_dir, "r");

      // Read file data and send to the client 1033 bytes at a time
      memset(&file_buffer, 0, BUFSIZE);// Set Buffer To Zeros
      while((block_size = fread(file_buffer, sizeof(char), BUFSIZE, file)) > 0)
      {
        // send the file data to the client
        send(echoClient_socket, file_buffer, block_size, 0);
      }

    /******** Close File and Socket after File Data is Sent to Client ********/
    fclose(file);
    printf("File \"%s\" was sent from server.\n", filename);
    close(echoClient_socket);
    }

    return 0;
}
