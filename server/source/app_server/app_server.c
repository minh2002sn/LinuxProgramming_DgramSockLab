#include "app_server.h"
#include "app_config.h"
#include "error_checker.h"
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

#define LOG_SOCK_INFO(name, addr)                                                                      \
    printf("\n====================\n" name ":\n    address: %s\n    port: %d\n====================\n", \
           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))

typedef struct
{
    struct sockaddr_in addr;
    int fd;
} socket_data_t;
static socket_data_t *gh_server;

void app_server_init(int port_no)
{
    int ret = 0;
    int opt_val = 1;

    if(gh_server == NULL)
    {
        gh_server = (socket_data_t *)malloc(sizeof(socket_data_t));

        /* Init server address */
        gh_server->addr.sin_family = AF_INET;
        gh_server->addr.sin_port =  htons(port_no);
        gh_server->addr.sin_addr.s_addr = INADDR_ANY;
        LOG_SOCK_INFO("Server", gh_server->addr);

        /* Init socket */
        gh_server->fd = socket(AF_INET, SOCK_DGRAM, 0);
        ERROR_CHECK(gh_server->fd, "socket()");

        /* Prevent "Address already in use" error */
        ret = setsockopt(gh_server->fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
        ERROR_CHECK(ret, "setsockopt()");

        /* Bind socket with server address */
        ret = bind(gh_server->fd, (struct sockaddr *)&gh_server->addr, sizeof(gh_server->addr));
        ERROR_CHECK(ret, "bind()");
    }
}

void app_server_handle()
{
    int ret = 0;
    struct sockaddr_in h_client_addr;
    int client_addr_len = sizeof(h_client_addr);
    char rx_buff[BUFF_SIZE];
    int test_fd = 0;
    char file_buff[BUFF_SIZE];

    while(1)
    {
        int n = 0;
        long unsigned int num_tx_byte = 0;
        /* Receive request from client */
        ret = recvfrom(gh_server->fd, rx_buff, BUFF_SIZE, 0,
                    (struct sockaddr *)&h_client_addr, (socklen_t *)&client_addr_len);
        LOG_SOCK_INFO("Client", h_client_addr);
        ERROR_CHECK(ret, "recvfrom()");

        /* Transfer test.txt file */
        test_fd = open(DATA_FOLDER DATA_FILE_NAME, O_RDONLY);
        ERROR_CHECK(test_fd, "open()");
        while((n = read(test_fd, file_buff, BUFF_SIZE)) > 0)
        {
            ret = sendto(gh_server->fd, file_buff, n, 0,
                        (struct sockaddr *)&h_client_addr, sizeof(h_client_addr));
            ERROR_CHECK(ret, "sendto()");
            num_tx_byte += ret;
        }
        ret = sendto(gh_server->fd, NULL, 0, 0,
                    (struct sockaddr *)&h_client_addr, sizeof(h_client_addr));
        ERROR_CHECK(n, "sendto()");

        printf("Done transfer %ld bytes.\n", num_tx_byte);

        close(test_fd);
    }
}

void app_server_deinit()
{
    close(gh_server->fd);
    free(gh_server);
}
