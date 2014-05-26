#include "loadWavFile.hh"
#include <fstream>
#include <vector>
#include <ACGL/Utils/Log.hh>
#include <cstring>

using namespace std;
using namespace ACGL::Utils;

// based on code from Janis Born
ALuint createBufferFromWAV(const std::string &_filename) {
    ALuint buffer = 0;
    std::ifstream fileStream(_filename.c_str(), std::ios_base::in | std::ios_base::binary);

    if (fileStream.is_open() && fileStream.good()) {
        uint8_t identifier[4];
        uint16_t formatTag, channels, blockAlign, bitsPerSample;
        uint32_t fileSize, formatLength, sampleRate, bytesPerSecond, dataSize;

        fileStream.read((char*) identifier, 4); // "RIFF"
        if (std::strncmp((char*) identifier, "RIFF", 4)) {
            error() << _filename << " is not a RIFF file" << std::endl;
        }

        fileStream.read((char*) &fileSize, 4);

        fileStream.read((char*) identifier, 4); // "WAVE"
        if (std::strncmp((char*) identifier, "WAVE", 4)) {
            error() << _filename << " contains no WAVE section" << std::endl;
        }

        fileStream.read((char*) identifier, 4); // "fmt "
        if (std::strncmp((char*) identifier, "fmt ", 4)) {
            error() << _filename << ": expected fmt chunk not found" << std::endl;
        }

        fileStream.read((char*) &formatLength, 4);
        fileStream.read((char*) &formatTag, 2);
        fileStream.read((char*) &channels, 2);
        fileStream.read((char*) &sampleRate, 4);
        fileStream.read((char*) &bytesPerSecond, 4);
        fileStream.read((char*) &blockAlign, 2);
        fileStream.read((char*) &bitsPerSample, 2);

        // skip ahead if there is additional fmt header information (default length: 16)
        if (formatLength - 16u > 0) {
            fileStream.ignore(formatLength - 16u);
        }

        // skip over any additional chunks until we find the "data" chunk
        fileStream.read((char*) identifier, 4);
        while (std::strncmp((char*) identifier, "data", 4)) {
            // no "data" chunk, read the size and skip ahead
            uint32_t chunkSize;
            fileStream.read((char*) &chunkSize, 4);
            fileStream.ignore(chunkSize);

            // read the next chunk identifier
            if (!fileStream.good()) {
                error() << _filename << ": reachend end of file without finding data chunk" << std::endl;
            } else {
                fileStream.read((char*) identifier, 4);
            }
        }
        // we are now in the data chunk

        fileStream.read((char*) &dataSize, 4);

        if (formatTag != 1) {
            error() << _filename << " contains non-PCM WAVE data" << std::endl;
        }

        ALenum alFormat = 0;
        if (bitsPerSample == 8 && channels == 1)
            alFormat = AL_FORMAT_MONO8;
        if (bitsPerSample == 8 && channels == 2)
            alFormat = AL_FORMAT_STEREO8;
        if (bitsPerSample == 16 && channels == 1)
            alFormat = AL_FORMAT_MONO16;
        if (bitsPerSample == 16 && channels == 2)
            alFormat = AL_FORMAT_STEREO16;
        if (alFormat == 0) {
            error() << _filename << ": audio format not supported by OpenAL: " << bitsPerSample << " bits/sample, " << channels
                    << " channel(s)" << std::endl;
        }

        // Read the raw audio data and upload it to the OpenAL buffer
        std::vector<char> waveData = std::vector<char>(dataSize);
        fileStream.read(waveData.data(), dataSize);

        alGenBuffers(1, &buffer);
        alBufferData(buffer, alFormat, waveData.data(), dataSize, sampleRate);
    } else {
        error() << "Could not open " << _filename << std::endl;
    }

    return buffer;
}

