// Copyright 2016 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <grpc++/grpc++.h>

#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <atomic>

#include "google/cloud/speech/v1/cloud_speech.grpc.pb.h"
#include "alsapp/microphone.hpp"


using google::cloud::speech::v1::RecognitionConfig;
using google::cloud::speech::v1::Speech;
using google::cloud::speech::v1::StreamingRecognizeRequest;
using google::cloud::speech::v1::StreamingRecognizeResponse;

using alsapp::Microphone;


static std::atomic_bool microphone_on(true);
static std::string stop_word("stop");

// Write the audo in 1 second chunks
static void
microphone_main(
    grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
                                      StreamingRecognizeResponse> *streamer
)
{
    StreamingRecognizeRequest request;
    Microphone::period_type buffer[Microphone::size_buffer_msec(500)];

    std::cout << "sizeof(buffer): " << sizeof(buffer) << std::endl;

    std::size_t size_read;

    Microphone microphone;

    do {
        size_read = microphone.read(buffer);

        // And write the chunk to the stream.
        request.set_audio_content(&buffer[0],
                                  size_read);

        std::cout << "Sending " << size_read / 1024 << "k bytes." << std::endl;

        streamer->Write(request);
    } while (microphone_on);

    streamer->WritesDone();
}

int
main()
{
    StreamingRecognizeRequest request;

    // configure audio format
    auto *streaming_config   = request.mutable_streaming_config();
    auto *recognition_config = streaming_config->mutable_config();

    recognition_config->set_language_code("en");
    recognition_config->set_sample_rate_hertz(16000);
    recognition_config->set_encoding(RecognitionConfig::LINEAR16);

    // Create a Speech Stub connected to the speech service.
    auto channel = grpc::CreateChannel("speech.googleapis.com",
                                       grpc::GoogleDefaultCredentials());
    std::unique_ptr<Speech::Stub> speech(Speech::NewStub(channel));

    // Begin a stream.
    grpc::ClientContext context;
    auto streamer = speech->StreamingRecognize(&context);

    // Write the first request, containing the config only.
    streaming_config->set_interim_results(true);
    streamer->Write(request);

    // The microphone thread writes the audio content.
    std::thread microphone_thread(&microphone_main,
                                  streamer.get());

    // Read responses.
    StreamingRecognizeResponse response;
    while (streamer->Read(&response)) {  // Returns false when no more to read.
        // Dump the transcript of all the results.
        for (int r = 0; r < response.results_size(); ++r) {
            auto result = response.results(r);

            std::cout << "Result stability: " << result.stability() << std::endl;

            for (int a = 0; a < result.alternatives_size(); ++a) {
                auto alternative = result.alternatives(a);

                std::cout << alternative.confidence() << '\t'
                          << alternative.transcript() << std::endl;

                if (alternative.transcript().find(stop_word) != std::string::npos)
                    microphone_on = false;
            }
        }
    }

    grpc::Status status = streamer->Finish();

    microphone_thread.join();

    const int exit_status = !status.ok();

    if (exit_status)
        std::cerr << status.error_message() << std::endl; // RPC failure

    return exit_status;
}
// [END speech_streaming_recognize]
