//
//  AudioInput.h
//  SimpleVOIP
//
//  Created by Сергей Сейтов on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#ifndef AudioInput_h
#define AudioInput_h

#include <AudioToolbox/AudioToolbox.h>
#include "RingBuffer.h"

class AudioInput {
    
    AUGraph     _augraph;
    AudioUnit   _input;
    
    static OSStatus AudioInputCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);
    
public:
    AudioInput();
    ~AudioInput();
    
    RingBuffer ringBuffer;
    
    void start();
    void stop();
    void mute(bool isMute);
    void finish();
};

#endif /* AudioInput_hpp */
