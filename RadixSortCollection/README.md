A collection of radix sorts for array and for std::list, for c++17 compiler.  
-------------------------------------------------------------------------------------------

Following are the different usages for the RadixSort functions.  
**Examples for each can be found in main.cpp file**.

Notes:  
  1. Make sure that the type has a move cctor and a move copy assignment,  
     for avoiding cctors/assignments being performed during the sort.  
  2. Provide usable_memory if you don't want radix to allocate memory dynamically.  
     See details about the usable memory size in the API (radix_sort.h).

Usages:  
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
  
