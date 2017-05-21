//
//  AudioOutput.cpp
//  SimpleVOIP
//
//  Created by Sergey Seitov on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "AudioOutput.h"
#include "AudioUtils.h"

#define BUS_COUNT    8

#pragma mark - AudioOutput private

OSStatus AudioOutput::renderInput(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber,
                               UInt32 inNumberFrames,
                               AudioBufferList *ioData)
{
    AudioOutput* context = (AudioOutput*)inRefCon;
    Bus bus = context->_audioBus.find(inBusNumber);
    if (bus != context->_audioBus.end()) {
        if (bus->second->wasDied()) {
            context->enableInput(bus->second->number, false);
            delete bus->second;
            context->_audioBus.erase(bus);
        } else {
            int numSamples = ioData->mBuffers[0].mDataByteSize / sizeof(int16_t);
            int16_t* samples = (int16_t*)ioData->mBuffers[0].mData;
            bus->second->_ringBuffer.read(samples, numSamples);
        }
    }
    return noErr;
}

void AudioOutput::enableInput(uint32_t busNum, bool isEnable)
{
    if (isEnable) {
        printf("++++++++++++++ bus %d turn on\n", busNum);
    } else {
        printf("-------------- bus %d turn off\n", busNum);
    }
    CheckError(AudioUnitSetParameter(_mixer,
                                     kMultiChannelMixerParam_Enable,
                                     kAudioUnitScope_Input,
                                     busNum,
                                     (AudioUnitParameterValue)isEnable, 0), "kMultiChannelMixerParam_Enable failed");
}

#pragma mark - AudioOutput public

AudioOutput::AudioOutput()
{
    CheckError(NewAUGraph(&_augraph), "NewAUGraph failed");
    
    // Add output component
    AudioComponentDescription output_desc;
    output_desc.componentType = kAudioUnitType_Output;
    output_desc.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
    output_desc.componentFlags = 0;
    output_desc.componentFlagsMask = 0;
    output_desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    AUNode outputNode;
    CheckError(AUGraphAddNode(_augraph, &output_desc, &outputNode), "AUGraphAddNode failed");
    
    // Add mixer component
    AudioComponentDescription mixer_desc;
    mixer_desc.componentType = kAudioUnitType_Mixer;
    mixer_desc.componentSubType = kAudioUnitSubType_MultiChannelMixer;
    mixer_desc.componentFlags = 0;
    mixer_desc.componentFlagsMask = 0;
    mixer_desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    AUNode mixerNode;
    CheckError(AUGraphAddNode(_augraph, &mixer_desc, &mixerNode), "AUGraphAddNode failed");
    
    // Connect a node's output to a node's input
    CheckError(AUGraphConnectNodeInput(_augraph, mixerNode, 0, outputNode, 0), "AUGraphConnectNodeInput failed");
    
    // Open the graph AudioUnits are open but not initialized (no resource allocation occurs here)
    CheckError(AUGraphOpen(_augraph), "AUGraphOpen failed");
    CheckError(AUGraphNodeInfo(_augraph, mixerNode, NULL, &_mixer),  "AUGraphNodeInfo failed");
    CheckError(AUGraphNodeInfo(_augraph, outputNode, NULL, &_output),  "AUGraphNodeInfo failed");
    
    for (int i = 0; i < BUS_COUNT; ++i) {
        // Set a callback for the specified node's specified input
        AURenderCallbackStruct rcbs;
        rcbs.inputProc = &renderInput;
        rcbs.inputProcRefCon = this;
        
        CheckError(AUGraphSetNodeInputCallback(_augraph, mixerNode, i, &rcbs), "AUGraphSetNodeInputCallback failed");
        
        // Set input stream format to what we want
        CheckError(AudioUnitSetProperty(_mixer,
                                        kAudioUnitProperty_StreamFormat,
                                        kAudioUnitScope_Input, i,
                                        &AUDIO_FORMAT, sizeof(AUDIO_FORMAT)), "AudioUnitSetProperty failed");
        // Disconnect channel
        enableInput(i, false);
    }
    
    // Set output stream format
    CheckError(AudioUnitSetProperty(_mixer,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Output, 0,
                                    &AUDIO_FORMAT, sizeof(AUDIO_FORMAT)), "AudioUnitSetProperty failed");
    
    CheckError(AudioUnitSetProperty(_output,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Output, 1,
                                    &AUDIO_FORMAT, sizeof(AUDIO_FORMAT)), "AudioUnitSetProperty failed");
    
    // Initialise & start
    CheckError(AUGraphInitialize(_augraph), "AUGraphInitialize failed");
    start();
}

AudioOutput::~AudioOutput()
{
    stop();
    CheckError(AUGraphUninitialize(_augraph), "AUGraphUninitialize failed");
    CheckError(DisposeAUGraph(_augraph), "DisposeAUGraph failed");
    
    Bus bus = _audioBus.begin();
    while (bus != _audioBus.end()) {
        delete bus->second;
        bus = _audioBus.erase(bus);
    }
    
}

void AudioOutput::start()
{
    CheckError(AUGraphStart(_augraph), "AUGraphStart failed");
}

void AudioOutput::stop()
{
    CheckError(AUGraphStop(_augraph), "AUGraphStop failed");
}

void AudioOutput::write(int bufferID, uint8_t *buffer, int size)
{
    if (bufferID < BUS_COUNT) {
        if (_audioBus.find(bufferID) == _audioBus.end()) {
            _audioBus[bufferID] = new AudioBus(bufferID);
            enableInput(bufferID, true);
        }
        _audioBus[bufferID]->write(buffer, size);
    }
}

void AudioOutput::mute(bool isMute)
{
    if (isMute) {
        CheckError(AUGraphStop(_augraph), "AUGraphStop failed");
    } else {
        CheckError(AUGraphStart(_augraph), "AUGraphStart failed");
    }
}
