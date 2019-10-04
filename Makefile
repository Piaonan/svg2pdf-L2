EXEC=svg2pdf.exe
SOURCES=menu.c args.c xml.c pdf.c svg.c main.c 
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-W -Wall
 
.PHONY: default clean
 
default: $(EXEC)
 
menu.o: menu.c menu.h
args.o: args.c args.h
xml.o: xml.c xml.h
pdf.o: pdf.c pdf.h
svg.o: svg.c svg.h xml.h pdf.h 
main.o: main.c menu.h args.h svg.h 
 
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
 
$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ -lm
 
clean:
	rm -rf $(EXEC) $(OBJECTS) $(SOURCES:.c=.c~) $(SOURCES:.c=.h~) Makefile~