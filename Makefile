CC	= gcc
CXX	= g++
LDFLAGS = -lasound

TARGETS = sample latency list record demo

all: $(TARGETS)

list: list.c
	$(CC) $^ $(LDFLAGS) -o $@

record: record.c
	$(CC) $^ $(LDFLAGS) -o $@

sample: sample.c
	$(CC) $^ $(LDFLAGS) -o $@

latency: latency.c
	$(CC) $^ $(LDFLAGS) -lm -o $@

demo: demo.cpp
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGETS)
