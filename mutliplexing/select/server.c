#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8080
#define PROCESS_COUNT 256

int main(int argc, char *argv[])
{

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket option
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Assign IP and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen
    listen(server_fd, 3);

    // Fork processes
    pid_t pid;
    for (int i = 0; i < PROCESS_COUNT; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            break;
        }
    }

    // Child process
    if (pid == 0)
    {

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        while (1)
        {

            fd_set copy_readfds = readfds;

            int max_sd = server_fd;

            int activity = select(max_sd + 1, &copy_readfds, NULL, NULL, NULL);

            if (activity < 0 && errno != EINTR)
            {
                printf("select error");
            }

            // Check for new connections
            if (FD_ISSET(server_fd, &copy_readfds))
            {

                new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                // Add socket to set
                FD_SET(new_socket, &readfds);

                if (new_socket > max_sd)
                {
                    max_sd = new_socket;
                }

                printf("New connection , socket fd:%d , ip : %s , port :%d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                // char response[] = "HTTP/1.1 200 OK\r\n\r\n";

                // send(new_socket, response, sizeof(response), 0);

                // close(new_socket);
            }

            // Check for IO on existing sockets
            for (int i = 0; i <= max_sd; i++)
            {

                if (FD_ISSET(i, &copy_readfds))
                {

                    // Handle IO
                    // ...
                    printf("Connection , socket fd:%d , ip : %s , port :%d \n", i, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                }
            }
        }
    }

    if (pid != 0)
    {
        for (;;)
        {
            printf("Parent sleeping...");
            sleep(10);
        }
    }
    return 0;
}
