//
//  AudioInput.cpp
//  SimpleVOIP
//
//  Created by Сергей Сейтов on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "AudioInput.h"
#include "AudioUtils.h"

OSStatus AudioInput::AudioInputCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
    static AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0].mDataByteSize = inNumberFrames * AUDIO_FORMAT.mBytesPerFrame;
    bufferList.mBuffers[0].mNumberChannels = AUDIO_FORMAT.mChannelsPerFrame;
    if (bufferList.mBuffers[0].mData == nil) {
        bufferList.mBuffers[0].mData = malloc(1024);
    }

    AudioInput* context = (AudioInput*)inRefCon;
    CheckError(AudioUnitRender(context->_input, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, &bufferList), "AudioUnitRender failed");
    
    if (bufferList.mBuffers[0].mDataByteSize > 0) {
        int16_t* samples = (int16_t*)bufferList.mBuffers[0].mData;
        int numSamples = bufferList.mBuffers[0].mDataByteSize / sizeof(int16_t);
        context->ringBuffer.write(samples, numSamples);
    }
    
    return noErr;
}

AudioInput::AudioInput()
{
    AudioComponentDescription voiceUnitDesc;
    AUNode voiceNode;
    
    voiceUnitDesc.componentType = kAudioUnitType_Output;
    voiceUnitDesc.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
    voiceUnitDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    voiceUnitDesc.componentFlags = 0;
    voiceUnitDesc.componentFlagsMask = 0;
    
    CheckError(NewAUGraph(&_augraph), "NewAUGraph failed");
    CheckError(AUGraphAddNode(_augraph, &voiceUnitDesc, &voiceNode), "AUGraphAddNode failed");
    CheckError(AUGraphOpen(_augraph), "AUGraphOpen failed");
    CheckError(AUGraphNodeInfo(_augraph, voiceNode, NULL, &_input), "AUGraphNodeInfo failed");
    
    UInt32 enableInput        = 1;    // to enable input
    AudioUnitElement inputBus = 1;
    AudioUnitSetProperty(_input, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, inputBus, &enableInput, sizeof(enableInput));
    
    UInt32 enableOutput        = 0;    // to disable output
    AudioUnitElement outputBus = 0;
    AudioUnitSetProperty(_input, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, outputBus, &enableOutput, sizeof(enableOutput));
    
    CheckError(AudioUnitSetProperty(_input, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, inputBus, &AUDIO_FORMAT, sizeof(AUDIO_FORMAT)), "AudioUnitSetProperty (StreamFormat) failed");
    
    AURenderCallbackStruct callback;
    callback.inputProc = &AudioInputCallback;
    callback.inputProcRefCon = this;
    
    CheckError(AudioUnitSetProperty(_input, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, inputBus, &callback, sizeof(callback)), "AudioUnitSetProperty (InputCallback) failed");
    CheckError(AUGraphInitialize(_augraph), "AUGraphInitialize failed");
    
    start();
}

AudioInput::~AudioInput()
{
    stop();
    DisposeAUGraph(_augraph);
}

void AudioInput::start()
{
    CheckError(AUGraphStart(_augraph), "AUGraphStart failed");
}

void AudioInput::stop()
{
    CheckError(AUGraphStop(_augraph), "AUGraphStop failed");
    ringBuffer.flush();
}

void AudioInput::mute(bool isMute)
{
    if (isMute) {
        CheckError(AUGraphStop(_augraph), "AUGraphStop failed");
    } else {
        CheckError(AUGraphStart(_augraph), "AUGraphStart failed");
    }
}

void AudioInput::finish()
{
    ringBuffer.stop();
}
