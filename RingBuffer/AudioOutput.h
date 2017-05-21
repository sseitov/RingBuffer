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
#include <map>
#include <mutex>

typedef std::map<int, AudioBus*>::iterator Bus;

class AudioOutput {
    
    AUGraph                     _augraph;
    AudioUnit                   _mixer;
    AudioUnit                   _output;
    std::map<int, AudioBus*>   _audioBus;
    
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
    void write(int bufferID, uint8_t *buffer, int size);
    void mute(bool isMute);
};

#endif /* AudioOutput_h */
