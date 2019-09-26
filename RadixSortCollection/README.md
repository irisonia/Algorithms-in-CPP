Quickly sort or get a sorted view of large lists / arrays of any index-able type.</br>
Allowing the following features:
------------------------------------------------------------------------------------------------------------
- Sort std::list, std::vector and arrays of any indexable type, very quickly.
- Get a sorted 'view' of an array, by getting a sorted verstion of the indexes of the elements, keeping the original array unchanged.
  Useful for holding several sorted versions of the same array at the same time, each by a different sort critiria.</br>
  Note: The sorted indexes are invalidated with any change to the original array, therefore make sure to sort again after
  any change. No worries, it will sort again very quickly.  
  
Notes:
- Requires a C++17 compiler.
- Make sure type T has a move ctor and a move copy assignment.  
- You can provide memory for the algorythm to use, instead of radix allocating it dynamically.</br>
  See details about the memory size in the API (radix_sort_collection_api.h).   
- For sorting std::vector, send the address of its first element.

Included files:
- radix_sort_api.h : api for the client to use
- radix_sort_internal.h : implementation of the api
- test.cpp: running examples

Design considerations:
- Time efficiency (minimum copying, etc).
- Write as unified as possible logic for all the sorting options, for clarity and readability.

Usages examples:
---------------------------------------------------------------

- Sort a consecutive array of any integral type:  
  *RadixSort(arr, arr_size);  
  RadixSort(arr, arr_size, usable_memory);*

- Sort a consecutive array of any type T, that can be represented as an unsigned integral type.  
  *RadixSort(arr, size, type_to_unsigned_func);  
  RadixSort(arr, size, type_to_unsigned_func, usable_memory1, usable_memory2);*

- Get a sorted view of a consecutive array of any type T, that can be represented as an unsigned integral type.  
  *RadixSortIndexesOnly(arr, size, type_to_unsigned_func, output_arr);  
  RadixSortIndexesOnly(arr, size, type_to_unsigned_func, output_arr, usable_memory1, usable_memory2);*

- Sort a std::list of any type T, that can be represented as an unsigned integral type.  
  *RadixSort(lst, type_to_unsigned_func);  
  RadixSort(lst, type_to_unsigned_func, usable_memory1, usable_memory2)*
  
