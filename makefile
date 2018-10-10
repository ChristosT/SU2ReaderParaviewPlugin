test:
	g++ test.cpp SU2_mesh_io.cpp -p test

library:
	g++ -c -fPIC SU2_mesh_io.cpp
	ar rvs SU2_mesh_io.a SU2_mesh_io.o

clean:
	rm test SU2_mesh_io.o SU2_mesh_io.a

