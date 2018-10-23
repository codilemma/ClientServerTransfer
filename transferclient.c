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
#include <fcntl.h>

#define BUFSIZE 1033

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferclient [options]\n"                           \
    "options:\n"                                             \
    "  -s                  Server (Default: localhost)\n"    \
    "  -p                  Port (Default: 12041)\n"           \
    "  -o                  Output file (Default cs6200.txt)\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"output", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 12041;
    char *filename = "cs6200.txt";

    setbuf(stdout, NULL);

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:o:hx", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 's': // server
            hostname = optarg;
            break;
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'o': // filename
            filename = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    /******************** Collect Information About the Host Machine *********/
    // Source: https://github.com/zx1986/xSinppet/blob/master/unix-socket-practice/client.c 
    struct hostent* pHost_info;
    unsigned long host_address;

    // Get IP address from host name
    pHost_info = gethostbyname(hostname);
    // typecast host address into a long
    host_address = *(unsigned long *)(pHost_info->h_addr_list[0]);

    /******************** Create the client socket ***************************/
    int echoClient_socket = socket(AF_INET, SOCK_STREAM, 0);
    // error checking
    if (echoClient_socket == -1)
    {
      fprintf(stderr, "ERROR creating echoClient socket\n");
      exit(1);
    }

    /******************* Specify Socket Address *****************************/
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // Set address to Zeros
    server_address.sin_family      = AF_INET;           // Internet Protocol (AF_INET)
    server_address.sin_port        = htons(portno);     // Address port (16 bits)
    server_address.sin_addr.s_addr = host_address;      // Internet address (32 bits)

    /***************** Initiate a Connection to the socket ********************/
    int connection_status = connect(echoClient_socket, 
                                   (struct sockaddr *) &server_address, 
                                   sizeof(server_address));
    // error checking
    if (connection_status == -1)
    {
      fprintf(stderr, "ERROR connecting to the echoClient socket\n");
      exit(1);
    }

    /***************** Read Data from Server and Save to File ****************/
    char file_buffer[BUFSIZE];  // buffer to hold file contents
    char *file_output_dir;      // directory to store output file
    FILE *file;                 // file pointer
    size_t block_size;          // file block size.

    // Set output file directory
    file_output_dir = getcwd(file_buffer, BUFSIZE);
    strcat(strcat(file_output_dir, "/"), filename);

    // Open File location
    file = fopen(file_output_dir, "a");

    // Read file data from server 1033 bytes at a time
    memset(&file_buffer, 0, BUFSIZE);// Set Buffer To Zeros
    while((block_size = recv(echoClient_socket, file_buffer, BUFSIZE, 0)) > 0)
    {
      // write the data to the file system.
      fwrite(file_buffer, sizeof(char), block_size, file);
    }

    /******** Close File and Socket after Data is Recived and Stored *********/
    fclose(file);
    printf("File \"%s\" was written from server.\n", filename);
	close(echoClient_socket);

    return 0;    

}
