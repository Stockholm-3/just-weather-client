/**
 * @file client_cache.h
 * @brief Client-side caching system with file persistence
 *
 * This header provides a caching system for storing JSON responses with
 * configurable Time-To-Live (TTL) values. The cache uses both in-memory
 * storage and file-based persistence, with MD5 hashing for cache keys.
 *
 * Features:
 * - In-memory cache with LRU eviction
 * - File-based persistence for cache durability
 * - MD5 hashing of keys for filename generation
 * - TTL-based automatic expiration
 * - Maximum entry limit with automatic cleanup
 *
 * Cache files are stored in: src/client/cache/
 * File naming: MD5(key).json
 */

#ifndef CLIENT_CACHE_H
#define CLIENT_CACHE_H

#include <stddef.h>
#include <time.h>

#define CACHE_MAX_ENTRIES 50  ///< Default maximum number of cache entries
#define CACHE_DEFAULT_TTL 300 ///< Default TTL in seconds (5 minutes)

/**
 * @struct ClientCache
 * @brief Cache storage structure (opaque)
 *
 * Opaque structure that holds the cache state including entries list,
 * configuration, and metadata. Users should not access internal fields.
 */
typedef struct ClientCache ClientCache;

/**
 * @brief Creates a new cache instance
 *
 * Allocates and initializes a new ClientCache with the specified configuration.
 * The cache stores entries both in memory and on disk for persistence.
 * When the maximum number of entries is reached, the oldest entries are
 * evicted.
 *
 * @param max_entries Maximum number of cache entries to store.
 *                    When exceeded, oldest entries are removed (LRU).
 *                    Typical value: CACHE_MAX_ENTRIES (50).
 * @param default_ttl Default Time-To-Live in seconds for cache entries.
 *                    Entries older than TTL are considered expired.
 *                    Typical value: CACHE_DEFAULT_TTL (300 = 5 minutes).
 *
 * @return Pointer to the newly created ClientCache structure, or NULL if
 *         memory allocation fails
 *
 * @note The cache directory (src/client/cache/) is created automatically
 *       if it doesn't exist.
 *
 * @see client_cache_destroy()
 *
 * @par Example:
 * @code
 * ClientCache *cache = client_cache_create(CACHE_MAX_ENTRIES,
 * CACHE_DEFAULT_TTL); if (!cache) { fprintf(stderr, "Failed to create
 * cache\n"); return -1;
 * }
 * @endcode
 */
ClientCache* client_cache_create(size_t max_entries, time_t default_ttl);
/**
 * @brief Destroys a cache instance and frees all resources
 *
 * Frees all memory associated with the cache including all cached entries.
 * Safe to call with NULL pointer. Cache files on disk are NOT deleted.
 *
 * @param cache Pointer to the ClientCache structure to destroy (can be NULL)
 *
 * @note This function does not delete cache files from disk. To clear
 *       cache files, use client_cache_clear() before destroying.
 *
 * @see client_cache_create(), client_cache_clear()
 *
 * @par Example:
 * @code
 * ClientCache *cache = client_cache_create(50, 300);
 * // ... use cache ...
 * client_cache_destroy(cache);
 * @endcode
 */
void client_cache_destroy(ClientCache* cache);

/**
 * @brief Stores data in the cache
 *
 * Stores the provided JSON data in the cache with the specified key.
 * The data is stored both in memory and persisted to disk. The cache
 * key is hashed using MD5 to generate a filename for disk storage.
 *
 * If the cache is full (max_entries reached), the oldest entry is
 * automatically removed before adding the new one.
 *
 * @param cache Pointer to the ClientCache structure
 * @param key Cache key (typically an API endpoint or query identifier).
 *            Will be hashed with MD5 for filename generation.
 * @param json_data JSON string to cache. The data is copied internally.
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Data cached successfully
 * @retval -1 Failed to cache (invalid parameters, memory allocation failure,
 *            or file write error)
 *
 * @note If an entry with the same key exists, it will be updated with
 *       new data and timestamp.
 *
 * @note The json_data is copied, so the caller retains ownership of
 *       the original string.
 *
 * @see client_cache_get()
 *
 * @par Example:
 * @code
 * const char *json = "{\"temperature\": 20.5}";
 * if (client_cache_set(cache, "weather:stockholm", json) == 0) {
 *     printf("Data cached successfully\n");
 * }
 * @endcode
 */
int client_cache_set(ClientCache* cache, const char* key,
                     const char* json_data);
/**
 * @brief Retrieves data from the cache
 *
 * Looks up data in the cache by key. First checks in-memory cache, then
 * falls back to disk storage if not found in memory. Validates TTL before
 * returning data - expired entries are treated as cache misses.
 *
 * @param cache Pointer to the ClientCache structure
 * @param key Cache key to look up
 *
 * @return Dynamically allocated string containing the cached JSON data,
 *         or NULL if not found or expired. Caller must free the returned
 * string.
 *
 * @note The returned string is a copy - the caller owns it and must free it
 *       with free() when done.
 *
 * @note Returns NULL if:
 *       - Invalid parameters (cache or key is NULL)
 *       - Key not found in cache
 *       - Cached entry has expired (age > TTL)
 *       - File read error
 *
 * @see client_cache_set()
 *
 * @par Example:
 * @code
 * char *cached = client_cache_get(cache, "weather:stockholm");
 * if (cached) {
 *     printf("Cached data: %s\n", cached);
 *     free(cached);  // Don't forget to free!
 * } else {
 *     printf("Cache miss - fetching fresh data\n");
 * }
 * @endcode
 */
char* client_cache_get(ClientCache* cache, const char* key);

/**
 * @brief Clears all cache entries
 *
 * Removes all entries from both in-memory cache and deletes all cache
 * files from disk. The cache directory itself is preserved.
 *
 * @param cache Pointer to the ClientCache structure (safe to pass NULL)
 *
 * @note This operation is irreversible - all cached data is permanently
 * deleted.
 *
 * @note The cache can still be used after clearing - it starts empty.
 *
 * @par Example:
 * @code
 * // Clear all cached data
 * client_cache_clear(cache);
 * printf("Cache cleared\n");
 * @endcode
 */
void client_cache_clear(ClientCache* cache);

#endif
