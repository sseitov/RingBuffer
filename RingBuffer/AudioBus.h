//
//  AudioBus.h
//  RingBuffer
//
//  Created by Сергей Сейтов on 14.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#ifndef AudioBus_h
#define AudioBus_h

#include <AudioToolbox/AudioToolbox.h>
#include <opus/opus.h>
#include "AudioUtils.h"
#include "RingBuffer.h"

class AudioOutput;

class AudioBus {
    friend class AudioOutput;
    
    RingBuffer      _ringBuffer;
    OpusDecoder*    _opusDecoder;
    time_t          _lastAccess;
    int16_t         _decodeBuffer[OPUS_FRAME_SIZE];
    int             _decodeSamples;
    
public:
    AudioBus(int num);
    ~AudioBus();
    
    int number;
    bool wasDied();
    void write(uint8_t* buffer, int size);
};

#endif /* AudioBus_h */
