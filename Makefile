CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
SOURCES= mandel.c jpegrw.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mandel

all: $(EXECUTABLE) mandelmovie

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d) mandelmovie.d

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

mandelmovie: mandelmovie.o | $(EXECUTABLE)
	$(CC) mandelmovie.o -o $@ -lm

.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf $(OBJECTS) mandelmovie.o $(EXECUTABLE) mandelmovie *.d
