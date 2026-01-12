/**
 * @file hash_md5.h
 * @brief MD5 hashing utility with simplified interface
 *
 * This header provides a simple interface for MD5 hash generation based on
 * Alexander Peslyak's public domain MD5 implementation. It offers both binary
 * and hexadecimal string output formats.
 *
 * MD5 is commonly used for:
 * - Cache key generation (filename hashing)
 * - Data fingerprinting
 * - Checksums for non-cryptographic purposes
 *
 * @note MD5 is NOT cryptographically secure and should not be used for
 *       password hashing or security-critical applications. Use SHA-256 or
 *       better for security purposes.
 *
 * @note Based on Alexander Peslyak's public domain MD5 implementation.
 *
 * Hash format:
 * - Binary: 16 bytes (128 bits)
 * - Hex string: 32 characters + null terminator (33 bytes total)
 */

#ifndef HASH_MD5_H
#define HASH_MD5_H

#include <stddef.h>
#include <stdint.h>

#define HASH_MD5_STRING_LENGTH 33 ///< MD5 hex string length (32 chars + null)
#define HASH_MD5_BINARY_LENGTH 16 ///< MD5 binary hash length (16 bytes)

/**
 * @brief Calculates MD5 hash and returns it as a hexadecimal string
 *
 * Computes the MD5 hash of the provided data and stores it as a
 * null-terminated hexadecimal string (lowercase). This is the most
 * commonly used function for generating human-readable hashes.
 *
 * @param data Pointer to the input data to hash
 * @param data_size Size of the input data in bytes
 * @param output Buffer to store the hex string. Must be at least
 *               HASH_MD5_STRING_LENGTH (33) bytes.
 * @param output_size Size of the output buffer. Must be at least
 *                    HASH_MD5_STRING_LENGTH bytes.
 *
 * @return 0 on success, -1 on error
 * @retval 0 Hash computed successfully
 * @retval -1 Failed (invalid parameters or output buffer too small)
 *
 * @note The output is a null-terminated lowercase hexadecimal string.
 * @note The output buffer must be at least 33 bytes (32 hex chars + null).
 *
 * @see hash_md5_binary()
 *
 * @par Example:
 * @code
 * char hash[HASH_MD5_STRING_LENGTH];
 * const char *data = "Stockholm59.329318.0686";
 * if (hash_md5_string(data, strlen(data), hash, sizeof(hash)) == 0) {
 *     printf("MD5: %s\n", hash);
 *     // Output: "e7a8b9c0d1f2a3b4c5d6e7f8a9b0c1d2" (example)
 * }
 * @endcode
 *
 * @par Cache key generation example:
 * @code
 * char cache_key[HASH_MD5_STRING_LENGTH];
 * char query[256];
 * snprintf(query, sizeof(query), "weather:%s:%s", city, country);
 * hash_md5_string(query, strlen(query), cache_key, sizeof(cache_key));
 * // Use cache_key as filename: cache_key.json
 * @endcode
 */
int hash_md5_string(const void* data, size_t data_size, char* output,
                    size_t output_size);

/**
 * @brief Calculates MD5 hash and returns it as binary data
 *
 * Computes the MD5 hash of the provided data and stores it as raw binary
 * (16 bytes). Use this when you need the binary representation or want
 * to convert to hex string later with hash_md5_binary_to_string().
 *
 * @param data Pointer to the input data to hash
 * @param data_size Size of the input data in bytes
 * @param output Buffer to store the binary hash. Must be at least
 *               HASH_MD5_BINARY_LENGTH (16) bytes.
 *
 * @return 0 on success, -1 on error
 * @retval 0 Hash computed successfully
 * @retval -1 Failed (invalid parameters)
 *
 * @note The output is 16 bytes of binary data (not null-terminated).
 * @note For human-readable output, use hash_md5_string() directly.
 *
 * @see hash_md5_string(), hash_md5_binary_to_string()
 *
 * @par Example:
 * @code
 * unsigned char hash[HASH_MD5_BINARY_LENGTH];
 * const char *data = "Hello World";
 * if (hash_md5_binary(data, strlen(data), hash) == 0) {
 *     // hash now contains 16 bytes of binary MD5 data
 *     for (int i = 0; i < HASH_MD5_BINARY_LENGTH; i++) {
 *         printf("%02x", hash[i]);
 *     }
 *     printf("\n");
 * }
 * @endcode
 */
int hash_md5_binary(const void* data, size_t data_size, unsigned char* output);

/**
 * @brief Converts binary MD5 hash to hexadecimal string
 *
 * Converts a 16-byte binary MD5 hash to a null-terminated hexadecimal
 * string (lowercase). Useful when you have a binary hash from
 * hash_md5_binary() and need a string representation.
 *
 * @param binary Pointer to binary hash data (must be HASH_MD5_BINARY_LENGTH
 *               bytes)
 * @param output Buffer to store hex string. Must be at least
 *               HASH_MD5_STRING_LENGTH (33) bytes.
 * @param output_size Size of output buffer. Must be at least
 *                    HASH_MD5_STRING_LENGTH bytes.
 *
 * @return 0 on success, -1 on error
 * @retval 0 Conversion successful
 * @retval -1 Failed (invalid parameters or output buffer too small)
 *
 * @note The output is a null-terminated lowercase hexadecimal string.
 * @note For most use cases, hash_md5_string() is more convenient.
 *
 * @see hash_md5_binary(), hash_md5_string()
 *
 * @par Example:
 * @code
 * unsigned char binary[HASH_MD5_BINARY_LENGTH];
 * char hex[HASH_MD5_STRING_LENGTH];
 *
 * // First get binary hash
 * hash_md5_binary("Hello", 5, binary);
 *
 * // Then convert to hex string
 * if (hash_md5_binary_to_string(binary, hex, sizeof(hex)) == 0) {
 *     printf("Hex: %s\n", hex);
 * }
 * @endcode
 */
int hash_md5_binary_to_string(const unsigned char* binary, char* output,
                              size_t output_size);

#endif /* HASH_MD5_H */
