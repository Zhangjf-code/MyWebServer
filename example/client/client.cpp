#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>


int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345); // Change the port number as needed
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change the IP address as needed

    if ( connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0) {
        std::cerr << "Failed to connect to server." << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

// Send data to the server

    char rbuf[1024]= {};
    std::cout << "Enter message to send: ";
    std::cin.getline(rbuf, sizeof(rbuf));
    ssize_t bytes_sent = send(sockfd, rbuf, strlen(rbuf), 0);

    if (bytes_sent < 0) {
        std::cerr << "Failed to send data to server." << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    char wbuf[1024]= {};
    // Receive data from the server
    ssize_t bytes_received = recv(sockfd, wbuf, sizeof(wbuf), 0);

    if (bytes_received < 0) {
        std::cerr << "Failed to receive data from server." << std::endl;
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Received message from server: " << wbuf << std::endl;
    getchar();

    // Receive data from the server
    bytes_received = recv(sockfd, wbuf, sizeof(wbuf), 0);

    if (bytes_received == 0) {
        std::cout << "Server closed the connection." << std::endl;
        close(sockfd);
    } else if (bytes_received < 0) {
        std::cerr << "Failed to receive data from server." << std::endl;
    } else {
        std::cout << "Received message from server: " << wbuf << std::endl;
    }
    getchar();
    return 0;
}