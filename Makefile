CC=g++

CFLAGS?=-O0 -I./gem5/include -static
OBJS=rejection.o

all: rejection

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< -std=c++11 -lpthread

rejection: $(OBJS)
	$(CC) -static -o $@ $(OBJS) -std=c++11 -lpthread

clean:
	rm -f *.o $(OBJS)
