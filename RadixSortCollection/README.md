A collection of time efficient radix sorts for large collections of integral types and any indexable T type, 
allowing you to:
------------------------------------------------------------------------------------------------------------
- Sort std::list, std::vector and arrays of integral types and any indexable type, very quickly.
- Get a sorted 'view' of an array, by getting a sorted verstion of the indexes of the elements, without actually changing the array.
  Useful for holding several sorted versions of the same array at the same time, each by a different sort critiria.</br>
  Having only the indexes sorted is extremely time efficient, because the copying of the elements that occures at the end of
  the regular sort, needs not be done.</br>
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
- Unify the logic as much as possible for all the sorts in this collection, divert only when necessary.
- Time efficiency (minimum copying, etc)
- Clear and simple


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
  
