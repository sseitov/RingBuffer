//
//  AudioBus.cpp
//  RingBuffer
//
//  Created by Сергей Сейтов on 14.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "AudioBus.h"

#define MAX_SILENCE 10

AudioBus::AudioBus(int num) : _lastAccess(0), number(num)
{
    int error = 0;
    _opusDecoder = opus_decoder_create(SAMPLE_RATE, CHANNELS_PER_FRAME, &error);
    if(error != OPUS_OK) {
        printf("Failed to create OPUS decoder (%d)\n", error);
    }
}

AudioBus::~AudioBus()
{
    if (_opusDecoder != NULL) {
        opus_decoder_destroy(_opusDecoder);
    }
}

bool AudioBus::wasDied()
{
    time_t currentTime = time(NULL);
    return ((currentTime - _lastAccess) > MAX_SILENCE);
}

void AudioBus::write(uint8_t* buffer, int size)
{
    if (_opusDecoder != NULL) {
        _lastAccess = time(NULL);
        if (size > 1) {
            _decodeSamples = opus_decode(_opusDecoder, buffer, size, _decodeBuffer, OPUS_FRAME_SIZE, 0);
        } else {
            _decodeSamples = opus_decode(_opusDecoder, NULL, 0, _decodeBuffer, OPUS_FRAME_SIZE, 0);
        }
        if (_decodeSamples > 0) {
            _ringBuffer.write(_decodeBuffer, _decodeSamples);
        }
    }
}
