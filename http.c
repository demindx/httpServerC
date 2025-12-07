#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Header *createHeader(char *contentType, int contentLength) {
    Header *header = malloc(sizeof(Header));
    if (header == NULL) {
        return NULL;
    }

    header->contentType = strdup(contentType);
    if (header->contentType == NULL) {
        free(header);
        return NULL;
    }

    header->contentLenght = contentLength;

    return header;
}

ResponseStartLine *createResponseStartLine(char *version, HttpStatus status) {
    ResponseStartLine *startLine = malloc(sizeof(ResponseStartLine));
    if (startLine == NULL) {
        return NULL;
    }

    char buff[100];
    sprintf(buff, "HTTP/%s", version);

    startLine->version = strdup(buff);
    if (startLine->version == NULL) {
        free(startLine);
        return NULL;
    }

    startLine->status = status;

    return startLine;
}

Response *createReponse(char *contentType, char *body, HttpStatus status) {
    Response *response = malloc(sizeof(Response));
    if (response == NULL) {
        return NULL;
    }

    response->body = strdup(body);

    if(response->body == NULL) {
        free(response);
        return NULL;
    }

    ResponseStartLine *startLine = createResponseStartLine("1.1", status);
    if (startLine == NULL) {
        freeResponse(response);
        return NULL;
    } 

    Header *header = createHeader(contentType, strlen(body));
    if (header == NULL) {
        freeResponse(response);
        freeResponseStartLine(startLine);
        return NULL;
    }

    response->header = header;
    response->startLine = startLine;

    return response;
}

char *serializeResponseStartLine(ResponseStartLine startLine) {
    char buff[256];
    char *statusMsg = "";

    switch (startLine.status) {
        case OK_200:
            statusMsg = "OK";
        break;
    }

    sprintf(buff, "%s %d %s\r\n", startLine.version, startLine.status, statusMsg);

    char *result = strdup(buff);
    if (result == NULL) {
        return NULL;
    }

    return result;
}

char *serializeHeader(Header header) {
    char buff[512];

    sprintf(buff, "Content-Type: %s\r\n" 
            "Content-Length: %d\r\n",
            header.contentType,
            header.contentLenght);

    char *result = strdup(buff);
    if (result == NULL) {
        return NULL;
    }

    return result;
}

char *serializeResponse(Response response) {
    char *startLine = serializeResponseStartLine(*response.startLine);
    if (startLine == NULL) {
        return NULL;
    }

    char *header = serializeHeader(*response.header);
    if (header == NULL) {
        free(startLine);
        return NULL;
    }

    int len = sizeof(char) *(strlen(startLine) + strlen(header) + (response.body ? strlen(response.body) : 0) + 1);

    char *result = malloc(len);
    if (result == NULL) {
        free(header);
        free(startLine);
        return NULL;
    }

    strcpy(result, startLine);
    strcat(result, header);

    if (response.body) {
        strcat(result, "\r\n");
        strcat(result, response.body);
    }

    free(header);
    free(startLine);
    return result;
}

void freeResponseStartLine(ResponseStartLine *startLine) {
    free(startLine->version);
    free(startLine);
}

void freeResponse(Response *response) {
    freeResponseStartLine(response->startLine);
    freeHeader(response->header);
    free(response->body);
    free(response);
}

void freeHeader(Header *header) {
    free(header->contentType);
    free(header);
}
