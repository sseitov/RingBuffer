//
//  AudioOutput.h
//  SimpleVOIP
//
//  Created by Sergey Seitov on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#ifndef AudioOutput_h
#define AudioOutput_h

#include "AudioBus.h"
#include <AudioToolbox/AudioToolbox.h>
#include <thread>

#define BUS_COUNT    8

class AudioOutput {
    
    AUGraph         _augraph;
    AudioUnit       _mixer;
    AudioUnit       _output;
    AudioBus        _audioBus[BUS_COUNT];
    std::thread*    _checkThread;
    
    static OSStatus renderInput(void *inRefCon,
                                AudioUnitRenderActionFlags *ioActionFlags,
                                const AudioTimeStamp *inTimeStamp,
                                UInt32 inBusNumber,
                                UInt32 inNumberFrames,
                                AudioBufferList *ioData);
    void enableInput(uint32_t busNum, bool isEnable);
   
public:
    AudioOutput();
    ~AudioOutput();
    
    void start();
    void stop();
    void write(uint8_t bufferID, uint8_t *buffer, int size);
    void mute(bool isMute);
};

#endif /* AudioOutput_h */
