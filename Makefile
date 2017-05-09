CC	= gcc
LDFLAGS = -lasound

all: sample latency list record

list: list.c
	$(CC) $^ $(LDFLAGS) -o $@

record: record.c
	$(CC) $^ $(LDFLAGS) -o $@

sample: sample.c
	$(CC) $^ $(LDFLAGS) -o $@

latency: latency.c
	$(CC) $^ $(LDFLAGS) -lm -o $@


