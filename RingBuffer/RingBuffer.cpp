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

RingBuffer::RingBuffer() : _capacity(POOL_SIZE), _start(0), _end(0), _stopped(false)
{
    _data = new Element[_capacity];
}

RingBuffer::~RingBuffer()
{
    stop();
    delete [] _data;
}

int RingBuffer::available()
{
    if (_start < _end) {
        return _end - _start;
    } else if (_start > _end) {
        return  _capacity - (_start - _end);
    } else {
        return 0;
    }
}

int RingBuffer::space()
{
    if (_end < _start) {
        return _start - _end - 1;
    } else if (_end > _start) {
        return _capacity - _end + _start - 1;
    } else {
        return _capacity - 1;
    }
}

void RingBuffer::log()
{
    printf("start %d, end %d, available %d, free space %d\n", _start, _end, available(), space());
}

void RingBuffer::read(Element* samples, int count)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _checkAvailable = count;
    _available.wait(lock, [this]() { return (isAvailable() || _stopped);});
    if (_stopped) return;
    
    for (int i = 0; i<count; i++) {
        samples[i] = _data[_start];
        _start = (_start + 1) % _capacity;
    }
    
    _space.notify_one();
}

void RingBuffer::write(Element* samples, int count)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _checkSpace = count;
    _space.wait(lock, [this]() { return (isSpace() || _stopped);});
    if (_stopped) return;
    
    for (int i = 0; i<count; i++) {
        _data[_end] = samples[i];
        _end = (_end + 1) % _capacity;
    }
    
    _available.notify_one();
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
    _space.notify_one();
    _available.notify_one();
}
