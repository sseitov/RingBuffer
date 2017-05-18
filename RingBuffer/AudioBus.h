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

class AudioBus {
    OpusDecoder*    _opusDecoder;
    time_t          _lastAccess;
    bool            _isOn;
    
public:
    AudioBus();
    ~AudioBus();
    
    RingBuffer  ringBuffer;
    
    bool isOn() { return _isOn; }
    bool wasDied();
    
    void write(uint8_t* buffer, int size);
    void finish();
};

#endif /* AudioBus_h */
