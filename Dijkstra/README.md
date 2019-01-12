- Compile with std=c++11.
- The algorithm prints the shortest path from one city to another in a directed, weighted graph.  
  For enjoying locality of reference, and working with integers rather than with strings, I have an adjacency matrix, and not
  an adjacency list. This would make this specific implementation better for the denser graphs rather than for the sparse ones.
- The output includes the route, the distance added by each city, and the total distance.
- The input must be completely valid, as the code depends on valid input, and does not protect against input mistakes.
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

