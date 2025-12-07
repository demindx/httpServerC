#pragma once

// =================== RESPONSE ==============================
// HTTP/1.1 400 Bad Request
// Date: Fri, 05 Dec 2025 16:32:09 GMT
// Server: Apache
// Content-Length: 226
// Connection: close
// Content-Type: text/html; charset=iso-8859-1
// =================== REQUEST  ==============================
// GET /index.html HTTP/1.1
// Host: www.example.com
// User-Agent: Mozilla/5.0
// Accept: text/html,application/xhtml+xml
// Accept-Language: en-US,en;q=0.9
// Connection: keep-alive
// Cookie: sessionId=abc123
//
// POST /api/login HTTP/1.1
// Host: api.example.com
// Content-Type: application/json
// Authorization: Bearer token123
// User-Agent: MyClient/1.0
// Content-Length: 42

// {"username":"john","password":"secret123"}/ Content-Length: 0
//

typedef struct {

} HttpServer;


typedef enum {
	GET,
	POST,
	PUT,
	PATCH,
	DELETE,
	HEAD,
	OPTIONS,
	CONNECT,
	TRACE
} HttpMethod;

typedef enum {
	CONTINUE_100 = 100,
	SWITCHING_PROTOCOLS_101 = 101,
	PROCESSING_102 = 102,
	EARLY_HINTS_103 = 103,

	OK_200 = 200,
	CREATED_201 = 201,
	ACCEPTED_202 = 202,
	NON_AUTHORITATIVE_INFORMATION_203 = 203,
	NO_CONTENT_204 = 204,
	RESET_CONTENT_205 = 205,
	PARTIAL_CONTENT_206 = 206,
	MULTI_STATUS_207 = 207,
	ALREADY_REPORTED_208 = 208,
	IM_USED_226 = 226,

	MULTIPLE_CHOICES_300 = 300,
	MOVED_PERMANENTLY_301 = 301,
	FOUND_302 = 302,
	SEE_OTHER_303 = 303,
	NOT_MODIFIED_304 = 304,
	USE_PROXY_305 = 305,
	TEMPORARY_REDIRECT_307 = 307,
	PERMANENT_REDIRECT_308 = 308,

	BAD_REQUEST_400 = 400,
	UNAUTHORIZED_401 = 401,
	PAYMENT_REQUIRED_402 = 402,
	FORBIDDEN_403 = 403,
	NOT_FOUND_404 = 404,
	METHOD_NOT_ALLOWED_405 = 405,
	NOT_ACCEPTABLE_406 = 406,
	PROXY_AUTHENTICATION_REQUIRED_407 = 407,
	REQUEST_TIMEOUT_408 = 408,
	CONFLICT_409 = 409,
	GONE_410 = 410,
	LENGTH_REQUIRED_411 = 411,
	PRECONDITION_FAILED_412 = 412,
	PAYLOAD_TOO_LARGE_413 = 413,
	URI_TOO_LONG_414 = 414,
	UNSUPPORTED_MEDIA_TYPE_415 = 415,
	RANGE_NOT_SATISFIABLE_416 = 416,
	EXPECTATION_FAILED_417 = 417,
	IM_A_TEAPOT_418 = 418,
	MISDIRECTED_REQUEST_421 = 421,
	UNPROCESSABLE_ENTITY_422 = 422,
	LOCKED_423 = 423,
	FAILED_DEPENDENCY_424 = 424,
	TOO_EARLY_425 = 425,
	UPGRADE_REQUIRED_426 = 426,
	PRECONDITION_REQUIRED_428 = 428,
	TOO_MANY_REQUESTS_429 = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE_431 = 431,
	UNAVAILABLE_FOR_LEGAL_REASONS_451 = 451,

	INTERNAL_SERVER_ERROR_500 = 500,
	NOT_IMPLEMENTED_501 = 501,
	BAD_GATEWAY_502 = 502,
	SERVICE_UNAVAILABLE_503 = 503,
	GATEWAY_TIMEOUT_504 = 504,
	HTTP_VERSION_NOT_SUPPORTED_505 = 505,
	VARIANT_ALSO_NEGOTIATES_506 = 506,
	INSUFFICIENT_STORAGE_507 = 507,
	LOOP_DETECTED_508 = 508,
	NOT_EXTENDED_510 = 510,
	NETWORK_AUTHENTICATION_REQUIRED_511 = 511
} HttpStatus;

typedef struct {
	HttpMethod method;
	char *path;
	char *version;
} RequestStartLine;

typedef struct {
	char *version;
	HttpStatus status;
} ResponseStartLine;

typedef struct {
	char *contentType;
	int contentLenght;
} Header;

typedef struct {
	RequestStartLine *startLine;
	Header *header;
	char *body;
} Request;

typedef struct {
	ResponseStartLine *startLine;
	Header *header;
	char *body;
} Response;

Header *createHeader(char *contentType, int contentLength);

RequestStartLine *createRequestStartLine();
Request *createRequest();

ResponseStartLine *createResponseStartLine(char *version, HttpStatus status);
Response *createReponse(char *contentType, char *body, HttpStatus status);

char *serializeResponseStartLine(ResponseStartLine startLine);
char *serializeResponse(Response response);
char *serializeHeader(Header header);

void freeResponseStartLine(ResponseStartLine *startLine);
void freeResponse(Response *response);
void freeHeader(Header *header);
