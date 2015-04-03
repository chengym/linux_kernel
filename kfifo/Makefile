
CC=gcc
RM = rm -fr
SUBDIR = .
SRCS = $(wildcard $(SUBDIR)/*.c)
OBJS = $(SRCS:.c = $(SUBDIR)/.o)

.PHONY: all clean

all: kfifo
kfifo:
	$(CC) -o $@ -pthread -I. $(OBJS) 
	echo "finish building $@..."

%.o:%.c
	$(CC) -pthread -c $<

clean:
	@$(RM) kfifo *.o .depend
	@echo "finish cleaning ..."
