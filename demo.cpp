#include "alsapp/microphone.hpp"
#include <fstream>


#ifndef OUTPUT_FILE
#define OUTPUT_FILE    "output.raw"
#endif // #ifndef OUTPUT_FILE

#ifndef RECORD_SECONDS
#define RECORD_SECONDS 3
#endif // #ifndef RECORD_SECONDS

using alsapp::Microphone;

int
main()
{
    Microphone microphone;

    Microphone::period_type buffer[Microphone::size_buffer(RECORD_SECONDS)];

    std::ofstream output(OUTPUT_FILE,
                         std::ofstream::binary);

    std::size_t size_read =  microphone.read(buffer);


    output.write(reinterpret_cast<char *>(&buffer[0]),
                 size_read);

    return 0;
}
