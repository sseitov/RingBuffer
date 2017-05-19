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

typedef int16_t Element;

class RingBuffer {
    int						_capacity;      // maximum number of elements
    int						_start;			// index of oldest element
    int						_end;			// index at which to write new element
    bool					_stopped;
    Element*				_data;			// raw data
    
    std::mutex				_mutex;
    std::condition_variable _available;
    std::condition_variable _space;

    int     _checkAvailable;
    int     _checkSpace;
    int     available();
    int     space();
    
    bool    isAvailable() { return available() >= _checkAvailable; }
    bool    isSpace() { return space() >= _checkSpace; }

public:
    
    RingBuffer();
    ~RingBuffer();
    
    void    read(Element* samples, int count);
    void    write(Element* samples, int count);
    void    log();

    void    flush();
    void    stop();
};

#endif /* OutputRingBuffer_h */
