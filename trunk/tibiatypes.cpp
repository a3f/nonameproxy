#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "tibiatypes.h"
#include "networkmessage.h"

//TWord8
TWord8::TWord8 (uint8_t val)
{
        _val = val;
}

TWord8::TWord8 (NetworkMessage* msg)
{
        get (msg);
}

void TWord8::put (NetworkMessage* msg)
{
        msg->putU8 (_val); 
}

void TWord8::get (NetworkMessage* msg)
{
        msg->getU8 (_val);
}

uint8_t TWord8::getVal ()
{
        return _val;
}

void TWord8::show ()
{
        printf ("%d", _val);
}

//TWord16
TWord16::TWord16 (uint16_t val)
{
        _val = val;
}

TWord16::TWord16 (NetworkMessage* msg)
{
        get (msg);
}

void TWord16::put (NetworkMessage* msg)
{
        msg->putU16 (_val); 
}

void TWord16::get (NetworkMessage* msg)
{
        msg->getU16 (_val);
}

uint16_t TWord16::getVal ()
{
        return _val;
}

void TWord16::show ()
{
        printf ("%d", _val);
}
//TWord32
TWord32::TWord32 (uint32_t val)
{
        _val = val;
}

TWord32::TWord32 (NetworkMessage* msg)
{
        get (msg);
}

void TWord32::put (NetworkMessage* msg)
{
        msg->putU32 (_val); 
}

void TWord32::get (NetworkMessage* msg)
{
        msg->getU32 (_val);
}

uint32_t TWord32::getVal ()
{
        return _val;
}

void TWord32::show ()
{
        printf ("%d", _val);
}

//TString
TString::TString (NetworkMessage* msg)
{
        get (msg);
}

TString::TString (std::string& str)
{
        _str = str;
}

const std::string& TString::getString ()
{
        return _str;
}

uint32_t TString::getLen ()
{
        return _str.length ();
}

void TString::put (NetworkMessage* msg)
{
        uint16_t len = _str.length ();
        const uint8_t* tmp = (const uint8_t*)_str.c_str ();
        msg->putU16 (len);
        msg->putN (tmp, len);
}
        
void TString::get (NetworkMessage* msg)
{
        uint16_t len;
        msg->getU16 (len);
        uint8_t tmp[len + 1];
        msg->getN (tmp, len);
        tmp[len] = '\0';
        _str.assign ((const char*)tmp);
}

void TString::show ()
{
        printf ("%s (len = %d)", _str.c_str (), _str.length ());
}

//TByteBuffer
TByteBuffer::TByteBuffer (uint32_t len, NetworkMessage* msg)
{
        get (len, msg);
}

TByteBuffer::TByteBuffer (uint32_t len)
{
        _buffer = new uint8_t[len];
        _len = len;
        //init the random number generator
        struct timeval tv;
        gettimeofday (&tv, NULL);
        srand (tv.tv_usec);
        //now fill the buffer, most of the time this is called
        //len will be < 8, so i havent bothered making it efficient
        for (uint32_t i = 0; i < len; i ++) {
                _buffer[i] = rand ();
        }
}

TByteBuffer::~TByteBuffer ()
{
        delete[] _buffer;
}

void TByteBuffer::put (NetworkMessage* msg)
{
        msg->putN (_buffer, _len);
}

void TByteBuffer::get (uint32_t len, NetworkMessage* msg)
{
        _buffer = new uint8_t[len];
        _len = len;
        msg->getN (_buffer, len);
}

void TByteBuffer::show ()
{
        printf ("%d random bytes", _len);
}

//TXTEAKey
TXTEAKey::TXTEAKey (NetworkMessage* msg)
{
        get (msg);
}

TXTEAKey::TXTEAKey (const uint32_t* key)
{
        memcpy (_keyBuffer, key, 16);
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i] = new TWord32 (key[i]);
        }
}

TXTEAKey::~TXTEAKey ()
{
        for (uint32_t i = 0; i < 4; i ++) {
                delete _key[i]; 
        }
}

void TXTEAKey::put (NetworkMessage* msg)
{
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i]->put (msg);
        }
}

const uint32_t* TXTEAKey::getKey ()
{
        return _keyBuffer;
}

void TXTEAKey::get (NetworkMessage* msg)
{
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i] = new TWord32 (msg);
                _keyBuffer[i] = _key[i]->getVal ();
        }
}

void TXTEAKey::show ()
{
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i]->show ();
                printf (" ");
        }
}
        
//TCharacter
TCharacter::TCharacter (NetworkMessage* msg)
{
        get (msg);
}

TCharacter::TCharacter (std::string& name, std::string& pwd,
        uint32_t ip, uint16_t port)
{
        _name = new TString (name);
        _pwd = new TString (pwd);
        _ip = new TWord32 (ip);
        _port = new TWord16 (port);
}

TCharacter::~TCharacter ()
{
        delete _name;
        delete _pwd;
        delete _ip;
        delete _port;
}

void TCharacter::put (NetworkMessage* msg)
{
        _name->put (msg);
        _pwd->put (msg);
        _ip->put (msg);
        _port->put (msg);
}
        
void TCharacter::get (NetworkMessage* msg)
{
        _name = new TString (msg);
        _pwd = new TString (msg);
        _ip = new TWord32 (msg);
        _port = new TWord16 (msg);
}

const std::string& TCharacter::getName ()
{
        return _name->getString ();
}

const std::string& TCharacter::getPwd ()
{
        return _pwd->getString ();
}

uint32_t TCharacter::getIp ()
{
        return _ip->getVal ();
}

uint16_t TCharacter::getPort ()
{
        return _ip->getVal ();
}

//TCharacterList

TCharacterList::TCharacterList (NetworkMessage* msg)
{
        get (msg);
}

TCharacterList::TCharacterList ()
{
        _nChars = new TWord8 ((uint8_t)0);
}

TCharacterList::~TCharacterList ()
{
        delete _nChars;
}

void TCharacterList::addChar (TCharacter& character)
{
        _charlist.push_back (character);
        uint32_t tmp = _nChars->getVal ();
        //i know this is ugly prehaps i will define the operators later
        delete _nChars;
        _nChars = new TWord8 (tmp + 1);
}

uint32_t TCharacterList::getNChars ()
{
        return _nChars->getVal ();
}

const CharList& TCharacterList::getCharList ()
{
        return _charlist;
}

void TCharacterList::get (NetworkMessage* msg)
{
        _nChars = new TWord8 (msg);
        uint32_t n = _nChars->getVal ();
        for (uint32_t i = 0; i < n; i ++) {
                _charlist.push_back (TCharacter (msg));
        }
}

