COMPILER=g++
BINARY=a.out
OPT_LEVEL=g

all: main.o
	g++ *.o -o $(BINARY) -lm -lpthread -lX11

%.o: %.cpp
	g++ $< -c -O$(OPT_LEVEL) --std=gnu++11

run: $(BINARY)
	./$(BINARY)

clean:
	rm -rf *.o $(BINARY)
