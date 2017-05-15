//
//  AudioUtils.c
//  SimpleVOIP
//
//  Created by Сергей Сейтов on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "AudioUtils.h"

AudioStreamBasicDescription AUDIO_FORMAT = {
    .mFormatID = kAudioFormatLinearPCM,
    .mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked,
    .mChannelsPerFrame = CHANNELS_PER_FRAME,
    .mSampleRate = SAMPLE_RATE,
    .mBitsPerChannel = 16,
    .mFramesPerPacket = 1,
    .mBytesPerFrame = 2,
    .mBytesPerPacket = 2
};

void CheckError(OSStatus error, const char *operation)
{
    if (error == noErr) return;
    
    char errorString[20];
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[1]) && isprint(errorString[2]) && isprint(errorString[3]) && isprint(errorString[4])) {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else {
        snprintf(errorString, sizeof(errorString), "%d", (int)error);
    }
    
    printf("Critical Error: %s (%s)\n", operation, errorString);
    exit(1);
}
