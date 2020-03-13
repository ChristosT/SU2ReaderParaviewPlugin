CPP_FLAGS=-Wall -O3
test: test.cpp SU2_mesh_io.cpp SU2_mesh_io.h
	g++ ${CPP_FLAGS} test.cpp SU2_mesh_io.cpp -o test

library:
	g++ ${CPP_FLAGS} -c -fPIC SU2_mesh_io.cpp -std=c++11
	ar rvs SU2_mesh_io.a SU2_mesh_io.o

clean:
	rm test SU2_mesh_io.o SU2_mesh_io.a

get_test_data:
	wget https://su2code.github.io/docs_files/square.su2
