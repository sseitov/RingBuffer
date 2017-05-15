//
//  AudioUtils.h
//  SimpleVOIP
//
//  Created by Сергей Сейтов on 01.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#ifndef AudioUtils_h
#define AudioUtils_h

#include <AudioToolbox/AudioToolbox.h>

#define OPUS_FRAME_SIZE 480
#define SAMPLE_RATE 24000.0
#define CHANNELS_PER_FRAME 1

void CheckError(OSStatus error, const char *operation);
extern AudioStreamBasicDescription AUDIO_FORMAT;

#endif /* AudioUtils_h */
