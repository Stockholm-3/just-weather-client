/**
 * @file client_list.h
 * @brief Doubly linked list implementation
 *
 * This header provides a generic doubly linked list data structure with
 * support for insertion, removal, and traversal operations. The list can
 * store any pointer type (void*) and provides custom memory management
 * through optional free functions.
 *
 * Features:
 * - Doubly linked nodes (back and front pointers)
 * - Generic void* storage for any data type
 * - Index-based and reference-based operations
 * - Custom memory management with free functions
 * - Macro for easy iteration
 * - O(1) append, O(n) insert/get operations
 */

#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <stddef.h>

/**
 * @struct Node
 * @brief Linked list node structure
 *
 * Represents a single node in the doubly linked list. Each node contains
 * pointers to the previous (back) and next (front) nodes, as well as a
 * pointer to the stored item.
 */
typedef struct Node Node;
struct Node {
    Node* back;  /**< Pointer to the previous node (NULL for head) */
    Node* front; /**< Pointer to the next node (NULL for tail) */
    void* item;  /**< Pointer to the stored data */
};

/**
 * @struct LinkedList
 * @brief Doubly linked list structure
 *
 * Container structure for a doubly linked list. Maintains pointers to the
 * head and tail nodes for efficient access, and tracks the total size.
 */
typedef struct {
    Node*  head; /**< Pointer to the first node (NULL if empty) */
    Node*  tail; /**< Pointer to the last node (NULL if empty) */
    size_t size; /**< Number of nodes in the list */
} LinkedList;

/**
 * @def LinkedList_foreach
 * @brief Macro for iterating over all nodes in a linked list
 *
 * Convenience macro that creates a for loop to iterate through all nodes.
 * The loop variable is declared within the macro.
 *
 * @param list Pointer to the LinkedList to iterate
 * @param node Name of the loop variable (Node*) to use in the loop body
 *
 * @par Example:
 * @code
 * LinkedList *list = linked_list_create();
 * // ... add items ...
 * LinkedList_foreach(list, node) {
 *     printf("Item: %p\n", node->item);
 * }
 * @endcode
 */
#define LinkedList_foreach(list, node)                                         \
    for (Node * (node) = (list)->head; (node) != NULL; (node) = (node)->front)

/**
 * @brief Creates a new empty linked list
 *
 * Allocates and initializes a new LinkedList structure. The list starts
 * empty with head and tail set to NULL and size set to 0.
 *
 * @return Pointer to the newly created LinkedList, or NULL if memory
 *         allocation fails
 *
 * @see linked_list_dispose()
 *
 * @par Example:
 * @code
 * LinkedList *list = linked_list_create();
 * if (!list) {
 *     fprintf(stderr, "Failed to create list\n");
 *     return -1;
 * }
 * @endcode
 */
LinkedList* linked_list_create();

/**
 * @brief Gets a node at a specific index
 *
 * Returns a pointer to the Node at the specified index. Traverses the list
 * from the head, counting nodes until reaching the target index.
 *
 * @param list Pointer to the LinkedList
 * @param index Zero-based index of the node to retrieve
 *
 * @return Pointer to the Node at the specified index, or NULL if the index
 *         is out of bounds or list is NULL
 *
 * @note Time complexity: O(n) where n is the index
 *
 * @par Example:
 * @code
 * Node *node = linked_list_get_index(list, 2);
 * if (node) {
 *     printf("Item at index 2: %p\n", node->item);
 * }
 * @endcode
 */
Node* linked_list_get_index(LinkedList* list, size_t index);

/**
 * @brief Inserts an item at a specific index
 *
 * Inserts a new node containing the specified item at the given index.
 * Existing nodes at and after this index are shifted to the right.
 * If index equals the list size, the item is appended to the end.
 *
 * @param list Pointer to the LinkedList
 * @param index Zero-based index where the item should be inserted
 * @param item Pointer to the data to store in the new node
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Item inserted successfully
 * @retval -1 Failed (invalid parameters, index out of bounds, or memory
 *            allocation failure)
 *
 * @note Time complexity: O(n) where n is the index
 * @note To append at the end, use linked_list_append() for O(1) performance
 *
 * @see linked_list_append()
 *
 * @par Example:
 * @code
 * char *data = strdup("Hello");
 * if (linked_list_insert(list, 0, data) == 0) {
 *     printf("Inserted at beginning\n");
 * }
 * @endcode
 */
int linked_list_insert(LinkedList* list, size_t index, void* item);

/**
 * @brief Appends an item to the end of the list
 *
 * Adds a new node containing the specified item at the end of the list.
 * This is more efficient than using insert with the last index.
 *
 * @param list Pointer to the LinkedList
 * @param item Pointer to the data to store in the new node
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Item appended successfully
 * @retval -1 Failed (invalid parameters or memory allocation failure)
 *
 * @note Time complexity: O(1)
 *
 * @see linked_list_insert()
 *
 * @par Example:
 * @code
 * char *data = strdup("World");
 * if (linked_list_append(list, data) == 0) {
 *     printf("Appended successfully\n");
 * }
 * @endcode
 */
int linked_list_append(LinkedList* list, void* item);

/**
 * @brief Removes a node from the list by reference
 *
 * Removes the specified node from the list and frees the node structure.
 * If a free_function is provided, it is called on the stored item to free
 * its memory. The node pointer becomes invalid after this operation.
 *
 * @param list Pointer to the LinkedList
 * @param item Pointer to the Node to remove
 * @param free_function Optional function to free the stored item. Pass NULL
 *                      if no cleanup is needed, or free for basic types.
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Node removed successfully
 * @retval -1 Failed (invalid parameters or node not found in list)
 *
 * @warning After calling this function, the node pointer is invalid and
 *          should not be accessed.
 *
 * @warning If free_function is NULL, the stored item is NOT freed and may
 *          leak memory if not handled elsewhere.
 *
 * @see linked_list_pop()
 *
 * @par Example:
 * @code
 * Node *node = linked_list_get_index(list, 0);
 * if (node) {
 *     linked_list_remove(list, node, free);  // Free the stored string
 * }
 * @endcode
 */
int linked_list_remove(LinkedList* list, Node* item,
                       void (*free_function)(void*));

/**
 * @brief Removes a node from the list by index
 *
 * Removes the node at the specified index from the list. This function
 * combines get_index and remove operations. If a free_function is provided,
 * it is called on the stored item.
 *
 * @param list Pointer to the LinkedList
 * @param index Zero-based index of the node to remove
 * @param free_function Optional function to free the stored item. Pass NULL
 *                      if no cleanup is needed, or free for basic types.
 *
 * @return 0 on success, -1 on failure
 * @retval 0 Node removed successfully
 * @retval -1 Failed (invalid parameters or index out of bounds)
 *
 * @note Time complexity: O(n) where n is the index
 *
 * @warning If free_function is NULL, the stored item is NOT freed and may
 *          leak memory if not handled elsewhere.
 *
 * @see linked_list_remove()
 *
 * @par Example:
 * @code
 * // Remove the first item and free its memory
 * if (linked_list_pop(list, 0, free) == 0) {
 *     printf("First item removed\n");
 * }
 * @endcode
 */
int linked_list_pop(LinkedList* list, size_t index,
                    void (*free_function)(void*));

/**
 * @brief Removes all nodes from the list
 *
 * Removes and frees all nodes in the list, leaving it empty. If a
 * free_function is provided, it is called on each stored item to free
 * its memory. The list structure itself is preserved and can be reused.
 *
 * @param list Pointer to the LinkedList
 * @param free_function Optional function to free stored items. Pass NULL
 *                      if no cleanup is needed, or free for basic types.
 *
 * @note After calling this function, the list is empty but still valid.
 *       You can continue to use it with append/insert operations.
 *
 * @warning If free_function is NULL, stored items are NOT freed and will
 *          leak memory if not handled elsewhere.
 *
 * @see linked_list_dispose()
 *
 * @par Example:
 * @code
 * // Clear all items and free their memory
 * linked_list_clear(list, free);
 * printf("List cleared, size: %zu\n", list->size);  // size is now 0
 * @endcode
 */
void linked_list_clear(LinkedList* list, void (*free_function)(void*));

/**
 * @brief Disposes the entire list and frees all memory
 *
 * Removes all nodes, frees all memory including the list structure itself,
 * and sets the list pointer to NULL to prevent dangling pointer issues.
 * If a free_function is provided, it is called on each stored item.
 *
 * @param list Double pointer to the LinkedList. After this call, *list will
 *             be set to NULL.
 * @param free_function Optional function to free stored items. Pass NULL
 *                      if no cleanup is needed, or free for basic types.
 *
 * @note Uses double pointer to automatically set the list variable to NULL,
 *       preventing accidental use of freed memory.
 *
 * @warning After calling this function, the list pointer is invalid. All
 *          operations on it will fail.
 *
 * @warning If free_function is NULL, stored items are NOT freed and will
 *          leak memory if not handled elsewhere.
 *
 * @see linked_list_create(), linked_list_clear()
 *
 * @par Example:
 * @code
 * LinkedList *list = linked_list_create();
 * // ... use list ...
 * linked_list_dispose(&list, free);  // list is now NULL
 * // list is NULL here, safe to check
 * if (list == NULL) {
 *     printf("List properly disposed\n");
 * }
 * @endcode
 */
void linked_list_dispose(LinkedList** list, void (*free_function)(void*));

#endif
