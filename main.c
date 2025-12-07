#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "http.h"

int main() 
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("socket: ");
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  
    addr.sin_port = htons(8080);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind: ");
        return -1;
    }

    if (listen(sock, 10) == -1) {
        perror("listen: ");
        return -1;
    }
    socklen_t len;
    printf("STARTING HTTP SERVER");
    fflush(stdout);

    while (true) {
        int sock_in = accept(sock, (struct sockaddr *)&addr, &len);
        if (sock_in < 0) {
            perror("accept failed");
            continue;
        }


        char buff[512];
        read(sock_in, buff, 512);

        printf("Recived request: %s", buff);
        fflush(stdout);

        Response *response = createReponse("text/html", "<h1>Hello world!</h1>", OK_200);

        char *serliazed = serializeResponse(*response);
        freeResponse(response);

        write(sock_in, serliazed, strlen(serliazed));
        free(serliazed);

        close(sock_in);
    }
    return 0;
}
