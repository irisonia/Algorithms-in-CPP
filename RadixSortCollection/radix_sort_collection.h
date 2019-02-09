/*  RadixSortCollection.h
 *  See README file for RadixSort usage examples.
 *  
 *  Notes:
 *  1. Ensure that the sorted type has move cctor and move copy assignment, 
 *     for avoiding cctors/assignments from being performed during the sort.
 *  2. Provide usable_memory if you don't want radix to allocate memory dynamically.
 *     See details about usable_memory in the API below.
*/

#ifndef RADIX_SORT_COLLECTION_H
#define RADIX_SORT_COLLECTION_H

#include <list>
#include <cstdint>

#include "radix_internal.h"

/* Description: Sort an array of any integral type.
 *
 * Parameters:
 * - arr: The array to sort.
 * - num_elements: Number of elements in arr.
 * - usable_memory: Supply if you don't want radix to allocate memory dynamically.
 *   If supplied, must be a consecutive memory chunk, at least the size of arr.
 *
 * Return: 0 for success, 1 in case of memory allocation failure.
 *
 * Memory complexity: Unless usable_memory is provided, the sort dynamically allocates an array of the same size as arr.
*/
template<class T>
int RadixSort(T *arr, size_t num_elements, void *usable_memory = nullptr) noexcept
{
    try {
        RadixIntegral(arr, num_elements, (T*)usable_memory);
        return 0;
    }
    catch (...) {
        return 1;
    }
}

/* Description: Sort an array of any type T, that can be represented as an unsigned integral type.
 * Radix sorts the unsigned integral values that correspond to the elements, and then rearranges the array accordingly.
 * The function refers to the unsigned integral type as U.
 *
 * Parameters:
 * - arr: The array to sort.
 * - num_elements: Number of elements in arr.
 * - T_to_unsigned: A function that returns an unsigned integral representation of an element.
 *   If the return type of the function is not unsigned, compilation fails.
 * - usable_mem1 and usable_mem2: Supply if you don't want radix to allocate memory dynamically.
 *   Each of the two usable memories, if supplied, must be a consecutive memory chunk, at least of the size:
 *   num_elements * (2 * sizeof(size_t))
 *
 * Return: 0 for success, 1 in case of memory allocation failure.
 *
 * Memory complexity: For each of usable_mem1 and usable_mem2 which is not supplied, the sort dynamically allocates
 * a consecutive array of the size: num_elements * (2 * sizeof(size_t)).
*/
template <class T, typename U, typename = std::enable_if_t<std::is_unsigned<U>::value>>
int RadixSort(
        T *arr,
        size_t num_elements,
        U(T_to_unsigned)(const T&),
        void *usable_mem1 = nullptr,
        void *usable_mem2 = nullptr) noexcept
{
    try {
        RadixConsecutive(
            arr,
            num_elements,
            T_to_unsigned,
            [=](RadixEntry<size_t> *sorted, RadixEntry<size_t> *helper_memory){
                RearrangeArr<T>(arr, num_elements, sorted, helper_memory);
            },
            usable_mem1,
            usable_mem2);

        return 0;
    }
    catch (...) {
        return 1;
    }
}

/* Description: Sort an array of any type T, that can be represented as an unsigned integral type, BUT without actually changing
 * the original array. Instead, fill an output array with the indexes of the elements from the original array, sorted.
 * Useful when needing different sort views of a single array at the same time.
 * Radix sorts an unsigned integral values that correspond to the elements, and then places the indexes in the output array accordingly.
 * The function refers to the unsigned integral type as U.
 *
 * Parameters:
 * - arr: Read-only array to "sort".
 * - num_elements: Number of elements in arr.
 * - T_to_unsigned: A function that returns an unsigned integral representation of an element.
 *   If the return type of the function is not unsigned, compilation fails.
 * - out: Where the indexes of the elements, sorted, will be placed.
 *   Must be of at least the size: num_elements * sizeof(size_t)
 * - usable_mem1 and usable_mem2: Supply if you don't want radix to allocate memory dynamically.
 *   Each of the two usable memories, if supplied, must be a consecutive memory chunk, at least of the size:
 *   num_elements * (2 * sizeof(size_t))
 *
 * Return: 0 for success, 1 in case of memory allocation failure.
 *
 * Memory complexity: For each of usable_mem1 and usable_mem2 which is not supplied, the sort dynamically allocates
 * a consecutive array of the size: num_elements * (2 * sizeof(size_t)).
*/
template <class T, typename U, typename = std::enable_if_t<std::is_unsigned<U>::value>>
int RadixSortIndexesOnly(
        const T *arr,
        size_t num_elements,
        U(*T_to_unsigned)(const T&),
        size_t *out,
        void *usable_mem1 = nullptr,
        void *usable_mem2 = nullptr) noexcept
{
    try {
        RadixConsecutive(
            arr,
            num_elements,
            T_to_unsigned,
            [=](RadixEntry<size_t> *sorted, RadixEntry<size_t> *) {
                size_t *p_out = out;
                for (size_t i = 0; i < num_elements; ++sorted, ++p_out, ++i) {
                    *p_out = sorted->m_first;
                }
            },
            usable_mem1,
            usable_mem2);

        return 0;
    }
    catch (...) {
        return 1;
    }
}

/* Description: Sort a std::list of any type T, that can be represented as an unsigned integral type.
 * Radix sorts an unsigned integral values that correspond to the elements, and then rearranges the list accordingly.
 * The function refers to the unsigned integral type as U.
 *
 * Parameters:
 * - lst: The list to sort.
 * - T_to_unsigned: A function that returns an unsigned integral representation of an element.
 *   If the return type of the function is not unsigned, compilation fails.
 * - usable_mem1 and usable_mem2: Supply if you don't want radix to allocate memory dynamically.
 *   Each of the two usable memories, if supplied, must be a consecutive memory chunk, at least of the size:
 *   lst.size() * (sizeof(std::list<T>::iterator) + sizeof(size_t))
 *
 * Return: 0 for success, 1 in case of memory allocation failure.
 *
 * Memory complexity: For each of usable_mem1 and usable_mem2 which is not supplied, the sort dynamically allocates
 * a consecutive array of the size lst.size() * (sizeof(std::list<T>::iterator) + sizeof(size_t)).
*/
template <class T, typename U, typename = std::enable_if_t<std::is_unsigned<U>::value>>
int RadixSort(
        std::list<T> &lst,
        U(*T_to_unsigned)(const T&),
        void *usable_mem1 = nullptr,
        void *usable_mem2 = nullptr) noexcept
{
    if (lst.empty()) return 0;

    try {
        RadixListImpl(lst, T_to_unsigned, usable_mem1, usable_mem2);
        return 0;
    }
    catch (...) {
        return 1;
    }
}

#endif // RADIX_SORT_COLLECTION_H
