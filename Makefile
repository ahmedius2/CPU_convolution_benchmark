all: conv_benchmark.cpp
	g++ -std=c++17 -march=native -O3 -ftree-vectorize -ftree-vectorizer-verbose=1 -o conv_benchmark conv_benchmark.cpp  -Iinclude -Wall -Wextra
clean:
	rm -r -f conv_benchmark *~
