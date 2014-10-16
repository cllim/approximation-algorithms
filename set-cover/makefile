EXECUTABLE = set-cover
CC = g++ -std=c++11 
CFLAGS = -Wall
COMMON = element.cpp set.cpp set-cover.cpp
SOURCE =  $(COMMON) main.cpp
TEST = $(COMMON) test-set-cover.cpp
DEPS = element.h set.h set-cover.h 
OBJ = $(SOURCE:.cpp=.o)
OBJ_TEST = $(TEST:.cpp=.o)

all: $(EXECUTABLE)

%.o: %.cpp $(DEPS)
	$(CC) -I/usr/local/include -c -o $@ $< $(CFLAGS) 

$(EXECUTABLE): $(OBJ)
	$(CC) -L/usr/local/lib -o $@ $^ $(CFLAGS) -lglpk

test: $(OBJ_TEST)
	$(CC) -I/usr/local/include -L/usr/local/lib -o $@ $^ $(CFLAGS) -lglpk

.PHONY: clean

clean:
	rm -f *.o $(EXECUTABLE) test