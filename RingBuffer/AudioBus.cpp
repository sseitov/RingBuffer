//
//  AudioBus.cpp
//  RingBuffer
//
//  Created by Сергей Сейтов on 14.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "AudioBus.h"

#define MAX_SILENCE 10

AudioBus::AudioBus() : _lastAccess(0), _isOn(false)
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
    if (_isOn) {
        time_t currentTime = time(NULL);
        _isOn = ((currentTime - _lastAccess) < MAX_SILENCE);
        return !_isOn;
    } else {
        return false;
    }
}

void AudioBus::write(uint8_t* buffer, int size)
{
    static int16_t  decodeBuffer[OPUS_FRAME_SIZE];
    int             decodeSamples;
    if (_opusDecoder != NULL) {
        _lastAccess = time(NULL);
        _isOn = true;
        if (size > 1) {
            decodeSamples = opus_decode(_opusDecoder, buffer, size, decodeBuffer, OPUS_FRAME_SIZE, 0);
        } else {
            decodeSamples = opus_decode(_opusDecoder, NULL, 0, decodeBuffer, OPUS_FRAME_SIZE, 0);
        }
        if (decodeSamples > 0) {
            ringBuffer.write(decodeBuffer, decodeSamples);
        }
    }
}

void AudioBus::finish()
{
    ringBuffer.stop();
}
