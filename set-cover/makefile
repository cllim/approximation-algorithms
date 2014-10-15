EXECUTABLE = set-cover
CC = g++ -std=c++11 
CFLAGS = -Wall
SOURCE = element.cpp set.cpp set-cover.cpp main.cpp
DEPS = element.h set.h set-cover.h 
OBJ = $(SOURCE:.cpp=.o)

all: $(EXECUTABLE)

%.o: %.cpp $(DEPS)
	$(CC) -I/usr/local/include -c -o $@ $< $(CFLAGS) 

$(EXECUTABLE): $(OBJ)
	$(CC) -L/usr/local/lib -o $@ $^ $(CFLAGS) -lglpk

.PHONY: clean

clean:
	rm -f *.o $(EXECUTABLE)