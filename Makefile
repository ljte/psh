CC=gcc
CFLAGS=-Wall
SOURCE_FILES=src/psh.c
OUTPUT=psh


all: compile run


compile:
	$(CC) $(SOURCE_FILES) -o $(OUTPUT)


run:
	./$(OUTPUT)