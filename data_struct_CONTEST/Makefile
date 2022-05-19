CXX=g++
FLAGS = -O2 -std=c++11

BIN_FOLDER=bin
SRC_FOLDER=src
FILES=${SRC_FOLDER}/main.cpp ${SRC_FOLDER}/AdjacencyList.cpp 
.PHONY: all clean

all:
	mkdir -p $(BIN_FOLDER);
	$(CXX) $(FILES) $(FLAGS) -o $(BIN_FOLDER)/exe;

clean:
	rm $(BIN_FOLDER)/*