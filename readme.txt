to compile: g++ DM3.cpp -llpsolve55 -lcolamd -ldl -o MyExec3
to execute: ./MyExec3 a2802.tsp
to generate the graph: dot -Tjpeg dm.dot -o dm.jpeg
the explanation of the variables:
the variables Xij == 1 if there is a connection from city i to city j
Xij == 0 if city i is not connected to city j
the variables Ui the order of the city i in the circuit
the explanation of the constraints:
1->the sum of the Xij == 0 ==> the city i can only be connected to a single city
2-> only one city can be connected to the city j
3-> the circuit must visit all the cities
4->the order of the city must be between 1 and n-1
