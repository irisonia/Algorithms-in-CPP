# Dijkstra README

- Compile with std=c++11.
- The algorithm prints the shortest path in a directed, weighted graph of cities.
- The output is the rout of cities that make the shortest distance, along with the distance added by each city, and the total.
- The input is composed of three parts, as described below. An example input can be foun in the file Input_example.

number_of_cities                *(3 in this example)*
city 1
city 2
city 3

numbr_of_adjacencies            *(5 in this example)*
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance
city name from, city name to, distance

city from, city to              *(the query itself)*
