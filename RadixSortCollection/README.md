C++17 radix sort functions for array and for std::list.  
-------------------------------------------------------------------------------------------

***When to use?
- When your type is indexable, but non-comparable.
- For an even faster execution than std::sort when sorting integral types.***

*See test.cpp for running examples.*

Notes:  
  - Requires a C++17 compiler.
  - Make sure the type has a move cctor and a move copy assignment, to prevent cctors/copy assignments during the sort.  
  - Provide usable_memory if you don't want radix to allocate memory dynamically. 
    See details about the usable memory size in the API (radix_sort_collection_api.h).  

Following are the different usages for the RadixSort functions:
---------------------------------------------------------------

- Sort a consecutive array of any integral type:  
  *RadixSort(arr, arr_size);  
  RadixSort(arr, arr_size, usable_memory);*

- Sort a consecutive array of any type, that can be represented as an unsigned integral type.  
  *RadixSort(arr, size, type_to_unsigned_func);  
  RadixSort(arr, size, type_to_unsigned_func, usable_memory1, usable_memory2);*

- Sort a consecutive array of any type, that can be represented as an unsigned integral type,  
  BUT without actually changing the original array. Instead, fill an output  
  array with the indexes of the elements from the original array, sorted.  
  Useful when needing different sort views of a single array at the same time.  
  *RadixSortIndexesOnly(arr, size, type_to_unsigned_func, output_arr);  
  RadixSortIndexesOnly(arr, size, type_to_unsigned_func, output_arr, usable_memory1, usable_memory2);*

- Sort a std::list of any type, that can be represented as an unsigned integral type.  
  *RadixSort(lst, type_to_unsigned_func);  
  RadixSort(lst, type_to_unsigned_func, usable_memory1, usable_memory2)*
  
