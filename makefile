test: test.cpp SU2_mesh_io.cpp
	g++ test.cpp SU2_mesh_io.cpp -o test

library:
	g++ -c -fPIC SU2_mesh_io.cpp
	ar rvs SU2_mesh_io.a SU2_mesh_io.o

clean:
	rm test SU2_mesh_io.o SU2_mesh_io.a

get_test_data:
	wget https://su2code.github.io/docs_files/square.su2
