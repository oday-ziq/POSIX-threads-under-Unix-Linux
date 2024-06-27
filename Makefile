CC = gcc
CFLAGS = -g -Wall
LIBS = -lglut -lGLU -lGL -lm -lrt

ARGS = args.txt

all: main production_lines opengl

main: main.o
	$(CC) $(CFLAGS) $< -o $@ 

production_lines: production_lines.o
	$(CC) $(CFLAGS) $< -o $@
    
opengl: opengl.o
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)    


run: main
	./main $(ARGS)

find:
	find -name '\tmp\message_queue*' -exec rm {} \;

clean:
	rm -f *.o main production_lines opengl
