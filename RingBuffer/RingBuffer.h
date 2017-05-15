//
//  OutputRingBuffer.h
//  RingBuffer
//
//  Created by Сергей Сейтов on 14.05.17.
//  Copyright © 2017 V-Channel. All rights reserved.
//

#ifndef RingBuffer_h
#define RingBuffer_h

#include <AudioToolbox/AudioToolbox.h>
#include <mutex>

#define POOL_SIZE   4
#define FRAME_SIZE  512

class RingBuffer {
    int						_count;			// maximum number of elements
    int						_bufferSize;	// size element of data
    int						_start;			// index of oldest element
    int						_end;			// index at which to write new element
    bool					_stopped;
    int16_t*				_data;			// raw data
    std::mutex				_mutex;
    std::condition_variable _overflow;
    std::condition_variable _empty;
   
public:
    
    RingBuffer();
    ~RingBuffer();
    
    bool isFull() { return (_end + 1) % _count == _start; }
    bool isEmpty() { return _end == _start; }
    
    void read(void (^data)(int16_t*));
    void write(void (^data)(int16_t*));

    int read(void* audioFrame);
    void write(void* audioFrame);
    
    void flush();
    void stop();
};

#endif /* OutputRingBuffer_h */
