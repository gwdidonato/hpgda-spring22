CXX=g++
FLAGS = -O2 -std=c++11

BIN_FOLDER=.
SRC_FOLDER=.
FILES=${SRC_FOLDER}/main.cpp
.PHONY: all clean

all:
	mkdir -p $(BIN_FOLDER);
	$(CXX) $(FILES) $(FLAGS) -o $(BIN_FOLDER)/graphgen;

clean:
	rm $(BIN_FOLDER)/*