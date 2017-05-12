CC	 = gcc
CXX	 = g++
CCFLAGS  =
CXXFLAGS = $(CCFLAGS) -I. -std=c++1z -g
LDFLAGS  = -lasound

PLAYER         = aplay
PLAYER_FLAGS   = --format=S16_LE --file-type=raw --channels=2 --rate=16000HZ
OUTPUT_FILE    = output.raw
PLAY           = $(PLAYER) $(PLAYER_FLAGS) $(OUTPUT_FILE) 
RECORD_SECONDS = 3

DEMO_FLAGS = -DOUTPUT_FILE=\"$(OUTPUT_FILE)\" -DRECORD_SECONDS=$(RECORD_SECONDS)
TARGETS    = sample latency list record demo

all: $(TARGETS)

list: list.c
	$(CC) $(CCFLAGS) $^ $(LDFLAGS) -o $@

record: record.c
	$(CC) $(CCFLAGS) $^ $(LDFLAGS) -o $@

sample: sample.c
	$(CC) $(CCFLAGS) $^ $(LDFLAGS) -o $@

latency: latency.c
	$(CC) $(CCFLAGS) $^ $(LDFLAGS) -lm -o $@

demo: demo.cpp
	$(CXX) $(CXXFLAGS) $(DEMO_FLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGETS) $(OUTPUT_FILE)

play:
	$(PLAY)

test: demo
	./demo
	$(PLAY)
