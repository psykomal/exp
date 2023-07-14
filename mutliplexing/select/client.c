// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <string.h>
// #include <fcntl.h>

// int main(int argc, char *argv[])
// {
//     struct sockaddr_in server_addr;
//     int sockfd, ret;
//     char buffer[1024];

//     if (argc != 3)
//     {
//         printf("Usage: %s <ip> <port>\n", argv[0]);
//         return 1;
//     }

//     // Create socket
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     // Specify server address
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(atoi(argv[2]));
//     inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

//     int flags = fcntl(sockfd, F_GETFL, 0);
//     fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
//     // Connect to server
//     ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
//     if (ret == -1)
//     {
//         perror("connect");
//         return 1;
//     }

//     // Send and receive data
//     // write(sockfd, "Hello World!", 12);
//     // ret = read(sockfd, buffer, 1024);
//     // buffer[ret] = '\0';
//     // printf("Received: %s\n", buffer);

//     close(sockfd);

//     return 0;
// }

#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

#define TIMEVAL_NSEC(ts) \
    ((ts)->tv_sec * 1000000000ULL + (ts)->tv_usec * 1000ULL)

uint64_t realtime_now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return TIMEVAL_NSEC(&tv);
}

int main()
{
    int port = 8080;

    // struct sockaddr_in sin4;
    // memset(&sin4, 0, sizeof(sin4));
    // sin4.sin_family = AF_INET;
    // sin4.sin_port = htons(port);
    // inet_pton(AF_INET, "127.0.0.1", &(sin4.sin_addr));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    struct sockaddr *sockaddr = (struct sockaddr *)&address;
    int sockaddr_len = sizeof(address);

    // int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    /* int one = 1; */
    /* setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, */
    /* 	   sizeof(one)); */

    uint64_t ta = realtime_now();
    uint64_t best_td = (uint64_t)-1LL;
    uint64_t sum = 0;
    uint64_t sum_sq = 0;
    int MAX = 16;
    int i;
    for (i = 0; i < MAX; i++)
    {
        int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        // int flags = fcntl(sd, F_GETFL, 0);
        // fcntl(sd, F_SETFL, flags | O_NONBLOCK);

        uint64_t t0 = realtime_now();
        int r = connect(sd, sockaddr, sockaddr_len);
        uint64_t t1 = realtime_now();
        if (r < 0 && errno != EINPROGRESS)
        {
            perror("connect()");
        }
        close(sd);

        int k;
        for (k = 0; k < 1000; k++)
        {
            sched_yield();
        }
        // usleep(1000*1000); // 100ms
        sleep(1);

        uint64_t td = t1 - t0;
        if (best_td > td)
            best_td = td;
        printf("took %.3fms\n", td / 1000000.);
        sum += td;
        sum_sq += td * td;
    }
    uint64_t tdd = realtime_now() - ta;
    printf("min=%.3fms tot=%.3fms\n", best_td / 1000000., tdd / 1000000.);

    double avg, dev, var;
    avg = (double)sum / (double)MAX;
    var = ((double)sum_sq / (double)MAX) - (avg * avg);
    dev = sqrt(var);

    printf("min %.3f avg %.3f var %.3f\n", best_td / 1000000., avg / 1000000., dev / 1000000.);
    return 0;
}