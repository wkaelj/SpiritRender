/**
 * @file spirit_vector.h
 * @author Kael Johnston (wkaelj@gmail.com)
 * @brief A vector implementation. Every macro takes a vector as the first
 * argument, which must be a reference to a vector of the correct type.
 *
 * Vector is a typed vector. If you know the type of the elements at compile
 * time use it, as it is much safer then dvector. dvector does not know the type
 * of an elment, only the size, making it very easy to use the wrong type when
 * insterting elements.
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#pragma once

#define VECTOR_RESIZE_AMOUNT 8

/**
 * @brief Initialize a new vector
 * @param type the type the vector will store
 *
 */
#define VECTOR(type)     \
    struct               \
    {                    \
        size_t size;     \
        size_t capacity; \
        type *at;        \
    }

/**
 * @brief Initialize a new vector
 * @param vector an pointer to the vector
 * @param size the initial size of the vector
 */
#define VECTOR_INIT(vector, init_size)                                    \
    do                                                                    \
    {                                                                     \
        (vector)->size     = 0;                                           \
        (vector)->capacity = init_size;                                   \
        (vector)->at       = malloc(init_size * sizeof((vector)->at[0])); \
    } while (false)

/**
 * @brief Delete a vector. If there is something that must be done to each
 * element, that must be done manually before destroying the vector. For
 * example, if the elements in the vector contain a pointer, that must be freed
 * before the pointer is deleted.
 * @param vector a reference to the vector
 *
 */
#define VECTOR_DELETE(vector)      \
    do                             \
    {                              \
        free((vector)->at);        \
        (vector)->at       = NULL; \
        (vector)->size     = 0;    \
        (vector)->capacity = 0;    \
    } while (false)

/**
 * @brief Resize a vector to the size of size_v. size_v must be larger then the
 * number of elements in the vector
 * @param vector a reference to the vector
 * @param size_v the size to make the vector. It must be greater then
 * vector->size
 *
 */
#define VECTOR_RESIZE(vector, size_v)                                  \
    do                                                                 \
    {                                                                  \
        assert(                                                        \
            size_v > (vector)->size &&                                 \
            "cannot make vector smaller then the number of elements"); \
        (vector)->at =                                                 \
            realloc((vector)->at, sizeof((vector)->at[0]) * (size_v)); \
        (vector)->capacity = size_v;                                   \
    } while (false)

/**
 * @brief Insert an item into the vector. it will become the index
 * @param vector the vector the element will inserted into
 * @param item the item to insert
 * @param index the index of the new item. Must be greater then 0 and less then
 * the size of the vector
 *
 */
#define VECTOR_INSERT(vector, item, index)                                 \
    do                                                                     \
    {                                                                      \
        assert(index < (vector)->size && "vector index is out of bounds"); \
        if ((vector)->capacity < (vector)->size + 1)                       \
            VECTOR_RESIZE(vector, (vector)->size + 1);                     \
        memmove(                                                           \
            &(vector)->at[index + 1],                                      \
            &(vector)->at[index],                                          \
            ((vector)->size - index) * sizeof((vector)->at[0]));           \
        (vector)->at[index] = item;                                        \
        ++(vector)->size;                                                  \
    } while (false)

/**
 * @brief Remove the element at index from the vector
 * @param vector the vector with the element to remove
 * @param index the index of the element to remove
 *
 */
#define VECTOR_POP(vector, index)                                         \
    do                                                                    \
    {                                                                     \
        assert(index < (vector)->size && "vector index out of bounds");   \
        memmove(                                                          \
            &(vector)->at[index],                                         \
            &(vector)->at[index + 1],                                     \
            (vector)->size - index * sizeof((vector)->at[0]));            \
        --(vector)->size;                                                 \
        if ((vector)->capacity > (vector)->size * 2)                      \
            VECTOR_RESIZE(vector, (vector)->size + VECTOR_RESIZE_AMOUNT); \
    } while (false)

/**
 * @brief Add a new element to the end of the vector
 *
 */
#define VECTOR_PUSH_BACK(vector, item)                                        \
    do                                                                        \
    {                                                                         \
        if ((vector)->size + 1 > (vector)->capacity)                          \
            VECTOR_RESIZE(vector, (vector)->capacity + VECTOR_RESIZE_AMOUNT); \
        ++(vector)->size;                                                     \
        (vector)->at[(vector)->size - 1] = item;                              \
    } while (false)

// Getters
#define VECTOR_SIZE(vector) ((vector)->size)
#define VECTOR_CAPACITY(vector) ((vector)->capacity)
#define VECTOR_AT(vector, index) ((vector)->at[index])

//
// DVECTOR
//

/**
 * @brief A dynamic vector can be recreated with a new type every time it
 * is initialized. It is not type safe, and if an element will be allowed as
 * long as it is the same size as the type the vector was initialized with.
 *
 */
typedef struct DVECTOR
{
    size_t capacity;
    size_t size;

    void *data;
    size_t elementSize;
} DVECTOR;


#define DVECTOR_AT(dvector, index, dvector_type) \
    (*((dvector_type *)DVECTOR_AT_REF(dvector, index)))

// dvector getters

/**
 * @brief Get the size of a dvector
 *
 * @param dvec the vector
 * @return size_t the size
 */
extern __always_inline size_t DVECTOR_SIZE(const DVECTOR *dvec)
{
    return dvec->size;
}

/**
 * @brief Get the capacity of the vector
 *
 * @param dvec the vector
 * @return size_t the number of spaces in the vector
 */
extern __always_inline size_t DVECTOR_CAPACITY(const DVECTOR *dvec)
{
    return dvec->capacity;
}

/**
 * @brief get a reference to an element of the vector
 *
 * @param dvec the vector
 * @param index the index of the element
 * @return void* a reference to the element at index
 */
extern __always_inline void *
DVECTOR_AT_REF(const DVECTOR *dvec, const unsigned index)
{
    return (void *)((char *)dvec->data + dvec->elementSize * index);
}

/**
 * @brief Initialize a dynanmic vector to hold a element of size item_size, with
 * a initial capacity of init_size
 * @param dvector the vector the initialize
 * @param item_size the size of the items stored in the vector
 * @param capacity number of elements the vector can hold initialy
 *
 */
extern __always_inline void
DVECTOR_INIT(DVECTOR *dvec, const size_t item_size, const size_t capacity)
{
    *dvec = (DVECTOR){
        .capacity    = capacity,
        .size        = 0,
        .elementSize = item_size,
        .data        = malloc(item_size * capacity),
    };
}

/**
 * @brief delete a dvector and its contents. If the members have pointer, they
 * will not be freed.
 *
 * @param dvec the vector
 *
 */
extern __always_inline void DVECTOR_DELETE(DVECTOR *dvec)
{
    free(dvec->data);
    *dvec = (DVECTOR){};
}

/**
 * @brief Resize a vector. It will fail if the size is less then the size of the
 * vector.
 *
 * @param dvec the vector
 * @param size the size to make the vector
 *
 */
extern __always_inline void DVECTOR_RESIZE(DVECTOR *dvec, const size_t size)
{
    assert(
        size > dvec->size &&
        "cannot make vector smaller then the number of elements");
    dvec->data     = realloc(dvec->data, dvec->elementSize * size);
    dvec->capacity = size;
}

/**
 * @brief Insert a new element into a dvector. this operation is very expensive.
 *
 * @param dvec the vector
 * @param item a pointer to the item to insert. it must be the same size as the
 * item the vector was initialized with
 * @param index the index the inserted element will have
 *
 */
extern __always_inline void
DVECTOR_INSERT(DVECTOR *dvec, const void *item, const unsigned index)
{
    assert(index < dvec->size && "Index out of bounds");
    if (dvec->capacity < dvec->size + 1)
        DVECTOR_RESIZE(dvec, dvec->capacity + VECTOR_RESIZE_AMOUNT);
    memmove(
        DVECTOR_AT_REF(dvec, index + 1),
        DVECTOR_AT_REF(dvec, index),
        dvec->size - index);
    memcpy(DVECTOR_AT_REF(dvec, index), item, dvec->elementSize);
    ++dvec->size;
}

/**
 * @brief Remove the element at index from the vector, and move all the elemnts
 * behind it back. This is an expensive operation.
 *
 * @param dvec the vector
 * @param index the index of the element which will be removed
 *
 */
extern __always_inline void DVECTOR_POP(DVECTOR *dvec, const unsigned index)
{
    assert(index < dvec->size && "Index out of bounds");
    memmove(
        DVECTOR_AT_REF(dvec, index),
        DVECTOR_AT_REF(dvec, index + 1),
        dvec->size - index * dvec->elementSize);
    --dvec->size;
    if (dvec->capacity > dvec->size * 2)
        DVECTOR_RESIZE(dvec, dvec->size + VECTOR_RESIZE_AMOUNT);
}

/**
 * @brief Add a new element to the end of a dynamic array. the element must be
 * the same type as the array was created with.
 * @param dvec a reference to a dvector initialized with the type of item
 * @param item a reference to the object to add
 *
 */
extern __always_inline void DVECTOR_PUSH_BACK(DVECTOR *dvec, const void *item)
{
    if (dvec->size + 1 > dvec->capacity)
        DVECTOR_RESIZE(dvec, dvec->size + VECTOR_RESIZE_AMOUNT);
    memcpy(DVECTOR_AT_REF(dvec, dvec->size), item, dvec->elementSize);
    ++dvec->size;
}