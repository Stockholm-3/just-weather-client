/**
 * @file http_client.h
 * @brief Simple HTTP/1.1 client implementation
 *
 * This header provides a simple HTTP client implementation built on top of
 * the TCP client. It supports HTTP GET requests with automatic URL parsing,
 * header processing, and chunked transfer encoding support. The client
 * automatically handles connection management and response parsing.
 *
 * @note This implementation currently supports HTTP only (not HTTPS).
 *       HTTPS support would require SSL/TLS integration.
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "client_tcp.h"

#include <stddef.h>

/**
 * @struct HttpClient
 * @brief HTTP client connection structure
 *
 * Structure that maintains the state of an HTTP client connection including
 * the underlying TCP connection, response data, and configuration.
 */
typedef struct {
    ClientTCP* tcp;           /**< Underlying TCP connection */
    char       url[1024];     /**< Current URL (reserved for future use) */
    int        status_code;   /**< HTTP status code from last response */
    char*      response_body; /**< Response body data (dynamically allocated) */
    size_t     response_size; /**< Size of response body in bytes */
    int timeout_ms; /**< Timeout for network operations in milliseconds */
} HttpClient;

/**
 * @brief Creates a new HTTP client instance
 *
 * Allocates and initializes a new HttpClient structure with the specified
 * timeout. The client creates an underlying TCP connection that will be
 * used for HTTP requests. The returned client must be destroyed with
 * http_client_destroy() when no longer needed.
 *
 * @param timeout_ms Timeout for network operations in milliseconds.
 *                   If <= 0, defaults to 5000ms (5 seconds).
 *
 * @return Pointer to the newly created HttpClient structure, or NULL if
 *         memory allocation fails or TCP client creation fails
 *
 * @see http_client_destroy()
 *
 * @par Example:
 * @code
 * HttpClient *client = http_client_create(10000); // 10 second timeout
 * if (!client) {
 *     fprintf(stderr, "Failed to create HTTP client\n");
 *     return -1;
 * }
 * @endcode
 */
HttpClient* http_client_create(int timeout_ms);

/**
 * @brief Destroys an HTTP client instance and frees all resources
 *
 * Closes any open connections, frees the response body buffer, destroys
 * the underlying TCP client, and frees the HttpClient structure memory.
 * Safe to call with NULL pointer. After calling this function, the pointer
 * should not be used anymore.
 *
 * @param client Pointer to the HttpClient structure to destroy (can be NULL)
 *
 * @see http_client_create()
 *
 * @par Example:
 * @code
 * HttpClient *client = http_client_create(5000);
 * // ... use client ...
 * http_client_destroy(client);
 * @endcode
 */
void http_client_destroy(HttpClient* client);

/**
 * @brief Performs an HTTP GET request
 *
 * Sends an HTTP GET request to the specified URL and receives the response.
 * The function handles the complete request/response cycle:
 * - Parses the URL to extract hostname, port, and path
 * - Establishes TCP connection to the server
 * - Sends HTTP/1.1 GET request with appropriate headers
 * - Receives and parses the HTTP response
 * - Handles chunked transfer encoding if present
 * - Validates the HTTP status code
 *
 * The response body is stored in the client structure and can be accessed
 * using http_client_get_body() and http_client_get_body_size().
 *
 * @param client Pointer to the HttpClient structure
 * @param url The URL to request (e.g., "http://example.com/api/data")
 *            Supports formats: http://hostname/path, http://hostname:port/path
 * @param error Optional pointer to store error message. If not NULL and an
 *              error occurs, will be set to a dynamically allocated string
 *              describing the error. Caller must free this string.
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Request completed successfully with valid HTTP status code
 * (200-599)
 * @retval -1 Request failed (see error parameter for details)
 *
 * @note The function will fail if:
 *       - Invalid parameters are provided (client is NULL, url is NULL)
 *       - URL parsing fails
 *       - Connection to server fails
 *       - Sending request fails
 *       - Receiving response fails
 *       - HTTP status code is invalid (< 200 or >= 600)
 *
 * @note The previous response body (if any) is freed before making a new
 * request.
 *
 * @warning This function blocks until the complete response is received or
 *          an error occurs. Use the timeout_ms parameter in
 * http_client_create() to limit blocking time.
 *
 * @see http_client_get_body(), http_client_get_body_size(),
 * http_client_get_status_code()
 *
 * @par Example:
 * @code
 * HttpClient *client = http_client_create(5000);
 * char *error = NULL;
 *
 * if (http_client_get(client, "http://api.example.com/data", &error) == 0) {
 *     printf("Status: %d\n", http_client_get_status_code(client));
 *     printf("Body: %s\n", http_client_get_body(client));
 * } else {
 *     fprintf(stderr, "Error: %s\n", error ? error : "Unknown");
 *     free(error);
 * }
 *
 * http_client_destroy(client);
 * @endcode
 */
int http_client_get(HttpClient* client, const char* url, char** error);

/**
 * @brief Gets the HTTP status code from the last response
 *
 * Returns the HTTP status code received from the most recent HTTP request.
 * Common status codes include:
 * - 200: OK
 * - 404: Not Found
 * - 500: Internal Server Error
 *
 * @param client Pointer to the HttpClient structure
 *
 * @return HTTP status code from last response, or 0 if client is NULL
 *         or no request has been made yet
 *
 * @see http_client_get()
 *
 * @par Example:
 * @code
 * int status = http_client_get_status_code(client);
 * if (status == 200) {
 *     printf("Success!\n");
 * } else if (status == 404) {
 *     printf("Resource not found\n");
 * }
 * @endcode
 */
int http_client_get_status_code(HttpClient* client);

/**
 * @brief Gets the response body from the last request
 *
 * Returns a pointer to the response body received from the most recent
 * HTTP request. The response body is null-terminated for convenience
 * when working with text responses.
 *
 * @param client Pointer to the HttpClient structure
 *
 * @return Pointer to the response body string (null-terminated), or NULL if
 *         client is NULL or no successful request has been made yet
 *
 * @note The returned pointer is owned by the HttpClient structure and will
 *       be freed when the client is destroyed or when a new request is made.
 *       Do not free this pointer manually.
 *
 * @note The response body remains valid until http_client_get() is called
 *       again or http_client_destroy() is called.
 *
 * @see http_client_get_body_size(), http_client_get()
 *
 * @par Example:
 * @code
 * const char *body = http_client_get_body(client);
 * if (body) {
 *     printf("Response: %s\n", body);
 * }
 * @endcode
 */
const char* http_client_get_body(HttpClient* client);

/**
 * @brief Gets the size of the response body from the last request
 *
 * Returns the size of the response body in bytes. This is useful when
 * working with binary data or when you need to know the exact size
 * without relying on null-termination.
 *
 * @param client Pointer to the HttpClient structure
 *
 * @return Size of the response body in bytes, or 0 if client is NULL
 *         or no successful request has been made yet
 *
 * @see http_client_get_body(), http_client_get()
 *
 * @par Example:
 * @code
 * size_t size = http_client_get_body_size(client);
 * const char *body = http_client_get_body(client);
 * printf("Received %zu bytes\n", size);
 * fwrite(body, 1, size, stdout);
 * @endcode
 */
size_t http_client_get_body_size(HttpClient* client);

#endif
