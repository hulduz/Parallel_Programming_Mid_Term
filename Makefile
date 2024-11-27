CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O3 -I/home/oul12/anaconda3/include/python3.12 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lm -fopenmp -lpython3.10

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = k_means

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)
