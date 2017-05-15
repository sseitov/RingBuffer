//
//  ViewController.m
//  RingBuffer
//
//  Created by Сергей Сейтов on 12.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#import "ViewController.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioUtils.h"
#include <AVFoundation/AVFoundation.h>
#include <CommonCrypto/CommonCrypto.h>

extern "C" {
    #include "voipcrypto.h"
    #include <opus/opus.h>
}

#include <vector>

@interface ViewController () {
    vcEncryptor*    _encryptor;
    AudioInput*     _audioInput;
    OpusEncoder*    _opusEncoder;
    
    vcDecryptor*    _decryptor;
    AudioOutput*     _audioOutput;
    
    std::vector<int16_t> _audioBuffer;

    dispatch_queue_t _senderQueue;
    std::mutex _senderMutex;
    bool _senderStopped;
    std::condition_variable _senderFinished;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    _senderStopped = true;
    
    // configure audio session
    NSError* error;
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionAllowBluetooth error:&error];
    [[AVAudioSession sharedInstance] setMode:AVAudioSessionModeVoiceChat error:nil];
    [[AVAudioSession sharedInstance] overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:&error];
    [[AVAudioSession sharedInstance] setInputGain:1.0 error:&error]; // ????
    
    [[AVAudioSession sharedInstance] setPreferredHardwareSampleRate: 24000.0 error: &error];
    [[AVAudioSession sharedInstance] setPreferredIOBufferDuration:0.02 error:&error];
    [[AVAudioSession sharedInstance] setActive:YES error:&error];
}

- (IBAction)start:(UIButton*)sender {
    if (!_senderStopped) {
        [self stopRecord:sender];
    } else {
        [self startRecord:sender];
    }
}

- (void)startRecord:(UIButton*)sender
{
    int8_t salt[] = {-106, -111, -100, -9, -25, -26, -21, 68, 6, 100, 45, -38, 109, -119, -76, -116};
    uint8_t key[16];
    if (CCKeyDerivationPBKDF(kCCPBKDF2, "voip", strlen("voip"), (uint8_t*)salt, sizeof(salt), kCCPRFHmacAlgSHA256, 1<<20, key, sizeof(key)) == kCCParamError)
    {
        printf("security error\n");
        return;
    }
    [sender setTitle:@"Stop" forState:UIControlStateNormal];

    _audioInput = new AudioInput();
    _encryptor = vcEncryptorCreate(key);
    
    int err = 0;
    _opusEncoder = opus_encoder_create(SAMPLE_RATE, CHANNELS_PER_FRAME, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        throw std::logic_error("Failed to create a OPUS encoder");
    } else {
        opus_encoder_ctl(_opusEncoder, OPUS_SET_DTX(1));
    }
    
    _audioOutput = new AudioOutput();
    _decryptor = vcDecryptorCreate(key);
    
    _senderQueue = dispatch_queue_create("NetworkingSender", DISPATCH_QUEUE_SERIAL);
    dispatch_async(_senderQueue, ^{
        printf("Start sender thread\n");
        _senderStopped = false;
        
        while (!_senderStopped) {
            if (_audioBuffer.size() >= OPUS_FRAME_SIZE) {
                [self sendBuffer];
            } else {
                _audioInput->ringBuffer.read( ^(int16_t* buffer){
                    _audioBuffer.insert(_audioBuffer.end(), buffer, buffer + FRAME_SIZE);
                });
            }
        }
        
        std::unique_lock<std::mutex> lock(_senderMutex);
        _senderStopped = true;
        _senderFinished.notify_one();
        printf("Sender thread stopped\n");
    });
}

- (void)sendBuffer {
    static uint8_t  entirePacket[1024];
    static uint8_t  opusBuffer[1024];
    int32_t length = opus_encode(_opusEncoder, &_audioBuffer[0], OPUS_FRAME_SIZE, opusBuffer, sizeof(opusBuffer));
    _audioBuffer.erase(_audioBuffer.begin(), _audioBuffer.begin() + OPUS_FRAME_SIZE);
    ssize_t encryptedLength = 0;
    if (length > 2) {
        unsigned char iv[16];
        int err = SecRandomCopyBytes(kSecRandomDefault, 16, iv);
        if (err == 0) {
            vcEncryptorEncrypt(_encryptor,
                               opusBuffer,
                               length,
                               iv);
            encryptedLength = _encryptor->encryptedLength;
            memcpy(entirePacket+1, _encryptor->encrypted, encryptedLength);
        }
    }
    
    vcDecryptorDecrypt(_decryptor, entirePacket+1);
    _audioOutput->write(entirePacket[0], _decryptor->decrypted, (int)_decryptor->decryptedLength);
}

- (void)stopRecord:(UIButton*)sender
{
    std::unique_lock<std::mutex> reciverLock(_senderMutex);
    if (!_senderStopped) {
        _senderStopped = true;
        _audioInput->finish();
        _senderFinished.wait(reciverLock);
    }
    
    vcDecryptorDestroy(_decryptor);
    delete _audioOutput;
    
    vcEncryptorDestroy(_encryptor);
    opus_encoder_destroy(_opusEncoder);
    delete _audioInput;

    [sender setTitle:@"Start" forState:UIControlStateNormal];
}

@end
