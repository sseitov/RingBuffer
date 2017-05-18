//
//  OutputRingBuffer.cpp
//  RingBuffer
//
//  Created by Сергей Сейтов on 14.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#include "RingBuffer.h"
#include "AudioUtils.h"

#define POOL_SIZE   1024*4

RingBuffer::RingBuffer() : _count(POOL_SIZE+1), _start(0), _end(0), _stopped(false)
{
    _data = (int16_t*)malloc(_count * sizeof(int16_t));
}

RingBuffer::~RingBuffer()
{
    free(_data);
}

void RingBuffer::read(int16_t* samples, int count)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _empty.wait(lock, [this]() { return (!isEmpty() || _stopped);});
    if (_stopped) return;

    for (int i = 0; i<count; i++) {
        samples[i] = _data[_start];
        _start = (_start + 1) % _count;
    }
    
    _overflow.notify_one();
}

void RingBuffer::write(int16_t* samples, int count)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _overflow.wait(lock, [this]() { return (!isFull() || _stopped);});
    if (_stopped) return;

    for (int i = 0; i<count; i++) {
        _data[_end] = samples[i];
        _end = (_end + 1) % _count;
    }
    
    _empty.notify_one();
}

void RingBuffer::flush()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _start = _end = 0;
}

void RingBuffer::stop()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _stopped = true;
    _overflow.notify_one();
    _empty.notify_one();
}
