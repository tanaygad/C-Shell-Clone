#include "functions.h"

void fetchManPage(const char* command) {
    struct addrinfo hints, *res, *p;
    int sockfd;
    char buffer[MAX_RESPONSE_SIZE];

    // Resolve DNS for man.he.net
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("man.he.net", "80", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Connect to the IP address
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break; // Connected successfully
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    // Send a GET request
    snprintf(buffer, sizeof(buffer), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command);
    send(sockfd, buffer, strlen(buffer), 0);

    // Read the response
    int numBytes;
    while ((numBytes = recv(sockfd, buffer, MAX_RESPONSE_SIZE - 1, 0)) > 0) {
        buffer[numBytes] = '\0';
        printf("%s", buffer);
    }

    // Close the socket
    close(sockfd);
}
