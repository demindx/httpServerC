#include "server.h"
#include "http.h"
#include "stream.h"

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE 8192

struct HttpServer {
  int socket;

  struct sockaddr_in addr;
  Router *router;
};

struct HandleParam {
  int client_fd;
  HttpServer *server;
};

static sig_atomic_t shouldRun;

static void signalHandler(int signo) {
  if (signo == SIGTERM) {
    signal(signo, SIG_IGN);
    shouldRun = 0;
  }
}

static void *handle(void *arg) {
  struct HandleParam *params = (struct HandleParam *)arg;

  if (!params->server)
    return 0;

  if (params->client_fd <= 0)
    return 0;

  fprintf(stdout, "[%02lx] handling a connection\n", pthread_self());
  fflush(stdout);

  while (1) {
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);

		Stream *stream = createStream(params->client_fd);
		Request *request = readRequest(stream);
		if (!request) continue;
		fprintf(stdout, "%d %s %s\n", request->method, request->version, request->uri);
    int r = read(params->client_fd, buff, sizeof(buff));

		if (r == 0) {
			continue;
		}

    fprintf(stdout, "[%02lx] read from connection: '%s'\n", pthread_self(),
            buff);
  }

  close(params->client_fd);
  fprintf(stdout, "[%02lx] connection closed\n", pthread_self());
  return 0;
}

HttpServer *createServer(char *addr, int port) {
  if (addr == NULL) {
    fprintf(stderr, "Incorrect address\n");
    return NULL;
  }

  if (port < 1024 || port > 65535) {
    fprintf(stderr, "Incorrect port\n");
    return NULL;
  }

  HttpServer *server = (HttpServer *)malloc(sizeof(HttpServer));
  if (server == NULL) {
    perror("Error while allocating HttpServer: ");
    return NULL;
  }

  if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error while creating socket: ");
    free(server);
    return NULL;
  }

  server->addr.sin_family = AF_INET;
  server->addr.sin_port = htons(port);
  server->addr.sin_addr.s_addr = inet_addr(addr);

  if (server->addr.sin_addr.s_addr == INADDR_NONE) {
    fprintf(stderr, "Invalid inet address\n");
    free(server);
    return NULL;
  }

  if (bind(server->socket, (struct sockaddr *)&server->addr,
           sizeof(server->addr)) != 0) {
    perror("Error while binding socket: ");
    free(server);
    return NULL;
  }

  return server;
}

void setRouter(HttpServer *server, Router *router) {
  if (router == NULL) {
    return;
  }

  server->router = router;
}

void serve(HttpServer *server) {
  shouldRun = 1;
  if (listen(server->socket, 5) != 0) {
    perror("Error while trying to listen socket: ");
    return;
  }

  signal(SIGTERM, signalHandler);

  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  memset(&client_addr, 0, sizeof(client_addr));

  while (shouldRun) {
    if ((client_fd = accept(server->socket, (struct sockaddr *)&client_addr,
                            &client_addr_len)) < 0) {

      perror("Error while accepting connection: ");
      break;
    }

    struct HandleParam *args = malloc(sizeof(struct HandleParam));
    args->client_fd = client_fd;
    args->server = server;

    pthread_t thread = {0};
    pthread_create(&thread, 0, handle, args);

    /* TODO: keep references to handler threads, and join them
     * before terminating */
  }
}
