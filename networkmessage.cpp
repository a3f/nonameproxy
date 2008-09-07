#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "networkmessage.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

NetworkMessage::NetworkMessage (uint32_t size)
{
        _buffer = new uint8_t[size];
        _curpos = 0;
        _size = size;
}

NetworkMessage::NetworkMessage (uint32_t size, uint8_t* buffer)
{
        _buffer = buffer;
        _curpos = 0;
        _size = size;
}

NetworkMessage::~NetworkMessage ()
{
        delete[] _buffer;
}

void NetworkMessage::prepRSAHeader ()
{
        _curpos = 2;
}

void NetworkMessage::prepHeader ()
{
        _curpos = 4;
}

void NetworkMessage::writeRSAHeader ()
{
        uint16_t packetSize = _curpos - 2;
        memcpy (_buffer, &packetSize, 2);
}

void NetworkMessage::writeHeader ()
{
        uint16_t plainSize = _curpos - 2;
        //once we have the plain size we must add random bytes for XTEA
        //a bit of a hack, roof to nearest 8
        struct timeval tv;
        gettimeofday (&tv, NULL);
        srand (tv.tv_usec);
        //calculate how many random bytes
        uint32_t r = (8 - plainSize % 8) % 8;
        for (uint32_t i = 0; i < r; i ++) {
                putU8 (rand ());
        }
        //finally we can calculate the the length of the 
        //encrypted packet
        uint16_t cryptSize = _curpos - 2;
        memcpy (_buffer, &cryptSize, 2);
        memcpy (&_buffer[2], &plainSize, 2);
}

uint8_t* NetworkMessage::getBuffer ()
{
        return _buffer;
}

void NetworkMessage::setPos (uint32_t pos)
{
        _curpos = pos;
}

bool NetworkMessage::isRSA ()
{
        uint16_t size = *(uint16_t*)_buffer;
        if (size % 8 != 0) {
                return true;
        } else {
                return false;
        }
}

bool NetworkMessage::isRSAEOF ()
{
        uint16_t eof = *(uint16_t*)_buffer + 2;
        if (_curpos >= eof) {
                return true;
        } else {
                return false;
        }
}

bool NetworkMessage::isXTEAEOF ()
{
        uint16_t eof = *(uint16_t*)&_buffer[2] + 4;
        if (_curpos >= eof) {
                return true;
        } else {
                return false;
        }
}

bool NetworkMessage::isEOF ()
{
        if (_curpos >= _size) {
                return true;
        } else {
                return false;
        }
}

bool NetworkMessage::getU8 (uint8_t& val)
{
        if (_curpos + 1 > _size) {
                printf ("network error: couldn't read U8\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 1);
        _curpos += 1;
        return true;
}

bool NetworkMessage::getU16 (uint16_t& val)
{
        if (_curpos + 2 > _size) {
                printf ("network error: couldn't read U16\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 2);
        _curpos += 2;
        return true;
}

bool NetworkMessage::getU32 (uint32_t& val)
{
        if (_curpos + 4 > _size) {
                printf ("network error: couldn't read U32\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 4);
        _curpos += 4;
        return true;
}

bool NetworkMessage::getN (uint8_t* dest, uint32_t n)
{
        if (_curpos + n > _size) {
                printf ("network error: couldn't read %d bytes\n", n);
                return false;
        }
        memcpy (dest, &_buffer[_curpos], n);
        _curpos += n;
        return true;
}

bool NetworkMessage::peekU8 (uint8_t& val)
{
        if (_curpos + 1 > _size) {
                printf ("network error: couldn't read U8\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 1);
        return true;
}

bool NetworkMessage::peekU16 (uint16_t& val)
{
        if (_curpos + 2 > _size) {
                printf ("network error: couldn't read U16\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 2);
        return true;
}

bool NetworkMessage::peekU32 (uint32_t& val)
{
        if (_curpos + 4 > _size) {
                printf ("network error: couldn't read U32\n");
                return false;
        }
        memcpy (&val, &_buffer[_curpos], 4);
        return true;
}

bool NetworkMessage::putU8 (uint8_t val)
{
        if (_curpos + 1 > _size) {
                printf ("network error: couldn't write U8\n");
                return false;
        }
        memcpy (&_buffer[_curpos], &val, 1);
        _curpos += 1;
        return true;
}
        
bool NetworkMessage::putU16 (uint16_t val)
{
        if (_curpos + 2 > _size) {
                printf ("network error: couldn't write U16\n");
                return false;
        }
        memcpy (&_buffer[_curpos], &val, 2);
        _curpos += 2;
        return true;
}
        
bool NetworkMessage::putU32 (uint32_t val)
{
        if (_curpos + 4 > _size) {
                printf ("network error: couldn't write U32\n");
                return false;
        }
        memcpy (&_buffer[_curpos], &val, 4);
        _curpos += 4;
        return true;
}

bool NetworkMessage::putN (const uint8_t* src, uint32_t n)
{
        if (_curpos + n > _size) {
                printf ("network error: couldn't write %d bytes\n", n);
                return false;
        }
        memcpy (&_buffer[_curpos], src, n);
        _curpos += n;
        return true;
}

void NetworkMessage::show () {
        uint16_t size = MIN (_size, *(uint16_t*)_buffer + 2);
        for (uint32_t i = 0; i <= size / 16; i ++) {
                for (uint32_t ii = 0; ii < 16; ii++) {
                        uint32_t p = i * 16 + ii;
                        if (p < size) {
                                printf ("%02X ", _buffer[p]);
                        } else {
                                printf ("   ");
                        }
                }
                printf ("        ");
                for (uint32_t ii = 0; ii < 16; ii++) {
                        uint32_t p = i * 16 + ii;
                        if (p < size) {
                                if (' ' <= _buffer[p] && _buffer[p] <= '~') {
                                        printf ("%c", _buffer[i * 16 + ii]);
                                } else {
                                        printf (".");
                                }
                        } else {
                                printf (" ");
                        }
                }
                printf ("\n");
        }
}

