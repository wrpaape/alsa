# Copyright 2016 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



# check required environment variables
ifeq (,$(GOOGLE_APPLICATION_CREDENTIALS))
$(error "environment variable 'GOOGLE_APPLICATION_CREDENTIALS' must be set to \
	 your Google Cloud service account credentials \
	 (see SpeechApi.README.md)")
endif # ifeq (,$(GOOGLE_APPLICATION_CREDENTIALS))

ifeq (,$(GOOGLEAPIS_GENS_PATH))
$(error "environment variable 'GOOGLEAPIS_GENS_PATH' must be set to \
	 your gRPC 'gens' directory (see [SpeechApi|Grpc].README.md)")
endif # ifeq (,$(GOOGLEAPIS_GENS_PATH))

# google protocol buffer sources
GOOGLEAPIS_CCS = $(shell find $(GOOGLEAPIS_GENS_PATH)/google/api	  \
		 	      $(GOOGLEAPIS_GENS_PATH)/google/rpc	  \
		  	      $(GOOGLEAPIS_GENS_PATH)/google/cloud/speech \
		  	      $(GOOGLEAPIS_GENS_PATH)/google/longrunning  \
			      -name '*.pb.cc')

SYSTEM ?= $(shell uname | cut -f 1 -d_) # use host system if not set

CXX = g++
CPPFLAGS += -I/usr/local/include -pthread -I$(GOOGLEAPIS_GENS_PATH) -I.
CXXFLAGS += -std=c++11

LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -lgrpc++_reflection
else
LDFLAGS += -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed
endif # ifeq ($(SYSTEM),Darwin)
LDFLAGS += -lprotobuf -lpthread -ldl -lasound


.PHONY: all
all: streaming_transcribe

googleapis.ar: $(GOOGLEAPIS_CCS:.cc=.o)
	ar r $@ $?

streaming_transcribe: streaming_transcribe.o googleapis.ar
	$(CXX) $^ $(LDFLAGS) -o $@

run_tests: all
	./streaming_transcribe

clean:
	rm -f *.o streaming_transcribe googleapis.ar $(GOOGLEAPIS_CCS:.cc=.o)
