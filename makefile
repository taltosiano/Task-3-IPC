
CC = gcc
CFLAGS = -Wall -Wextra

OBJS = ipv.o part_a.o stnc.o uds_stream.o
HDRS = ipv.h stnc.h

stnc: $(OBJS)
	$(CC) $(CFLAGS) -o stnc $(OBJS)

ipv.o: ipv.c ipv.h
	$(CC) $(CFLAGS) -c ipv.c

part_a.o: part_a.c $(HDRS)
	$(CC) $(CFLAGS) -c part_a.c

stnc.o: stnc.c $(HDRS)
	$(CC) $(CFLAGS) -c stnc.c

uds_stream.o: uds_stream.c
	$(CC) $(CFLAGS) -c uds_stream.c

clean:
	rm -f $(OBJS) stnc