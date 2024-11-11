#include <stdio.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h> 
#include <unistd.h>
#define BUFFER_SIZE 1001


//copy of my echoserver, nothing added just submitting so i can resubmit later sorry

void *handle_client(void *arg) { 
    int client_fd = *(int *)arg; 
    free(arg); 
    char buffer[BUFFER_SIZE]; 

    while (1) { 
        memset(buffer, 0, BUFFER_SIZE); 
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1); 
        buffer[bytes_read] = '\0'; 
        printf("client: %s\n", buffer);

        if (write(client_fd, buffer, bytes_read) != bytes_read) {
            printf("write fail");
            break;
        }
    } 
    close(client_fd); 
    return NULL; 
}  

int main(int argc, char *argv[]) 
{ 
    int server_fd, len; 
    int port = 0;

    sscanf(argv[2], "%d", &port);

    struct sockaddr_in servaddr, client; 
    server_fd = socket(AF_INET, SOCK_STREAM, 0); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
  
    if (server_fd < 0) { 
        printf("socket failed");
        exit(0);
    }

    if ((bind(server_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind fail, maybe wait a minute\n"); 
        exit(0); 
    } 

    if ((listen(server_fd, 5)) != 0) { 
        printf("listen fail\n"); 
        close(server_fd);
        exit(0); 
    }
    else
        printf("server listening\n");

    while (1) {
        len = sizeof(client); 
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr*)&client, &len); 
        printf("Connected\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_fd) != 0) {
            printf("fail to create thread");
            free(client_fd);
            close(*client_fd);
        } else {
            pthread_detach(thread_id);
        }
    }

    close(server_fd); 
    return 0;

} 
