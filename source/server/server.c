#include "server.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define LOG_SOCK_INFO(name, addr)                       \
printf(name " address: %s\n" name " port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))

#define LISTEN_BACKLOG                          50
#define BUFF_SIZE                               255
#define TEST_FILE_DIR                           "./input/test.txt"
#define SERVER_REQUEST                          "A"

void chat_func(int new_socket_fd);

int server_handle(int argc, char *argv[])
{
    int ret = 0;
    int port_no = 0;
    int server_fd = 0;
    struct sockaddr_in server_addr = {};
    struct sockaddr_in client_addr = {};
    unsigned long int client_addr_len = 0;
    int opt_val = 1;

    /* Read port number from command line */
    if(argc < 2)
    {
        printf("No port number is provided.\nCommand: ./server <port_number>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        port_no = atoi(argv[1]);
    }

    /* Init socket */
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_CHECK(server_fd, "socket()");
    
    /* Init server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port =  htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    LOG_SOCK_INFO("Server", server_addr);

    /* Prevent "Address already in use" error */
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, "bind()");

    /* Receive request from client */
    char recv_buff[BUFF_SIZE];
    ret = recvfrom(server_fd, recv_buff, BUFF_SIZE, 0,
                (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    ERROR_CHECK(ret, "recvfrom()");
    LOG_SOCK_INFO("Client", client_addr);

    /* Transfer test.txt file */
    int test_fd = open("./input/test.txt", O_RDONLY);
    ERROR_CHECK(test_fd, "open()");
    char file_buff[BUFF_SIZE];
    int n;
    while((n = read(test_fd, file_buff, BUFF_SIZE)) > 0)
    {
        ret = sendto(server_fd, file_buff, n, 0,
                    (struct sockaddr *)&client_addr, sizeof(client_addr));
        ERROR_CHECK(n, "write()");
    }

    // struct stat st;
    // ret = stat("./input/test.txt", &st);
    // ERROR_CHECK(ret, "stat()");
    // sendfile(client_fd, test_fd, NULL, st.st_size);

    close(test_fd);

    /* Close server socket */
    close(server_fd);

    return 0;
}
