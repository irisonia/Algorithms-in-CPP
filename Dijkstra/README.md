- Compile with std=c++11.
- Prints the shortest path from one city to another in a directed, weighted graph of city names and integral distances.  
  **This specific implementation is better for the denser graphs**, because for efficiency, I preferred locality of reference, 
  and working with integers rather than with strings.  
  Therefore, I have an adjacency matrix, and not an adjacency list, and an additional vector, that maps every city name to an index.
- The output includes the route, the distance added by each city, and the total distance.
- For efficiency, I chose to avoid exception handling, and rely on completely valid input.    
  An exception will be thrown (map out of range exception) in case of invalid input.
  
  The input includes three parts, as described below. An example input can be found in the input_example.txt file.
  Please mind that if you supply input using a redirected input file, rather than via the terminal (type/copy paste), then the  
  lines in the input file are expected to be terminated Linux style, and not Windows style.  
  The input structure is as follows:
```
number_of_cities
city 1
city 2
city 3

numbr_of_adjacencies
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance

query city from, query city to
```

Please let me know if you come across a bug, have a suggestion for improvement or any other helpful comment.  

**Thank you for visiting :)**


