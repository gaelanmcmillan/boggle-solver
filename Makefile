
test: lib/test.cpp boggle.o
	g++ -std=c++17 -O3 lib/test.cpp bin/boggle.o -o bin/calculate

boggle.o: lib/boggle.cpp
	g++ -std=c++17 -O3 -c lib/boggle.cpp -o bin/boggle.o

clean:
	rm /bin/**

solve:
	make && ./run_solver.sh

