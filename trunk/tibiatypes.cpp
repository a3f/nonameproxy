#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "tibiatypes.h"
#include "networkmessage.h"
#include "datreader.h"

//TWord8
TWord8::TWord8 (uint8_t val)
{
        _val = val;
}

TWord8::TWord8 (NetworkMessage* msg)
{
        get (msg);
}

TWord8::TWord8 (const TWord8& clone)
{
        _val = clone._val;
}

void TWord8::put (NetworkMessage* msg) const
{
        msg->putU8 (_val); 
}

void TWord8::get (NetworkMessage* msg)
{
        msg->getU8 (_val);
}

uint8_t TWord8::getVal () const
{
        return _val;
}

void TWord8::show () const
{
        printf ("%u", _val);
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

TWord16::TWord16 (const TWord16& clone)
{
        _val = clone._val;
}

void TWord16::put (NetworkMessage* msg) const
{
        msg->putU16 (_val); 
}

void TWord16::get (NetworkMessage* msg)
{
        msg->getU16 (_val);
}

uint16_t TWord16::getVal () const
{
        return _val;
}

void TWord16::show () const
{
        printf ("%u", _val);
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

TWord32::TWord32 (const TWord32& clone)
{
        _val = clone._val;
}

void TWord32::put (NetworkMessage* msg) const
{
        msg->putU32 (_val); 
}

void TWord32::get (NetworkMessage* msg)
{
        msg->getU32 (_val);
}

uint32_t TWord32::getVal () const
{
        return _val;
}

void TWord32::show () const
{
        printf ("%u", _val);
}

//TString
TString::TString (NetworkMessage* msg)
{
        get (msg);
}

TString::TString (const std::string& str)
{
        _str = str;
}

TString::TString (const TString& clone)
{
        _str = clone._str;
}

const std::string& TString::getString () const
{
        return _str;
}

uint32_t TString::getLen () const
{
        return _str.length ();
}

void TString::put (NetworkMessage* msg) const
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

void TString::show () const
{
        printf ("%s (len = %u)", _str.c_str (), _str.length ());
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

TByteBuffer::TByteBuffer (const TByteBuffer& clone)
{
        _len = clone._len;
        _buffer = new uint8_t[_len];
        memcpy (_buffer, clone._buffer, _len);
}

TByteBuffer::~TByteBuffer ()
{
        delete[] _buffer;
}

void TByteBuffer::put (NetworkMessage* msg) const
{
        msg->putN (_buffer, _len);
}

void TByteBuffer::get (uint32_t len, NetworkMessage* msg)
{
        _buffer = new uint8_t[len];
        _len = len;
        msg->getN (_buffer, len);
}

void TByteBuffer::show () const
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

TXTEAKey::TXTEAKey (const TXTEAKey& clone)
{
        memcpy (_keyBuffer, clone._keyBuffer, 16);
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i] = new TWord32 (*clone._key[i]);
        }
}

TXTEAKey::~TXTEAKey ()
{
        for (uint32_t i = 0; i < 4; i ++) {
                delete _key[i]; 
        }
}

void TXTEAKey::put (NetworkMessage* msg) const
{
        for (uint32_t i = 0; i < 4; i ++) {
                _key[i]->put (msg);
        }
}

const uint32_t* TXTEAKey::getKey () const
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

void TXTEAKey::show () const
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

TCharacter::TCharacter (const std::string& name, const std::string& world,
        uint32_t ip, uint16_t port)
{
        _name = new TString (name);
        _world = new TString (world);
        _ip = new TWord32 (ip);
        _port = new TWord16 (port);
}

TCharacter::TCharacter (const TCharacter& clone)
{
        _name = new TString (*clone._name);
        _world = new TString (*clone._world);
        _ip = new TWord32 (*clone._ip);
        _port = new TWord16 (*clone._port);
        clone._port->show (); printf (" "); _port->show (); printf ("\n");
}

TCharacter::~TCharacter ()
{
        delete _name;
        delete _world;
        delete _ip;
        delete _port;
}

void TCharacter::put (NetworkMessage* msg) const
{
        _name->put (msg);
        _world->put (msg);
        _ip->put (msg);
        _port->put (msg);
}
        
void TCharacter::get (NetworkMessage* msg)
{
        _name = new TString (msg);
        _world = new TString (msg);
        _ip = new TWord32 (msg);
        _port = new TWord16 (msg);
}

const std::string& TCharacter::getName () const
{
        return _name->getString ();
}

const std::string& TCharacter::getWorld () const
{
        return _world->getString ();
}

uint32_t TCharacter::getIp () const
{
        return _ip->getVal ();
}

uint16_t TCharacter::getPort () const
{
        return _port->getVal ();
}

void TCharacter::show () const
{
        printf ("name: "); _name->show (); printf (" ");
        printf ("world: "); _world->show (); printf (" ");
        printf ("ip: "); _ip->show (); printf (" ");
        printf ("port: "); _port->show (); printf (" ");
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

TCharacterList::TCharacterList (const TCharacterList& clone)
{
        _nChars = new TWord8 (*clone._nChars);
        //first make a copy of to avoid const iterators
        CharList tmp = clone._charlist;
        for (CharList::iterator i = tmp.begin ();
                i != tmp.end (); ++i) {
                _charlist.push_back (new TCharacter (*(*i)));
        }
}

TCharacterList::~TCharacterList ()
{
        delete _nChars;
        for (CharList::iterator i = _charlist.begin (); i != _charlist.end ();
                ++ i) {
                delete (*i);
        }
}

void TCharacterList::addChar (TCharacter* character)
{
        _charlist.push_back (character);
        uint32_t tmp = _nChars->getVal ();
        //i know this is ugly prehaps i will define the operators later
        delete _nChars;
        _nChars = new TWord8 (tmp + 1);
}

uint32_t TCharacterList::getNChars () const
{
        return _nChars->getVal ();
}

const CharList& TCharacterList::getCharList () const
{
        return _charlist;
}

void TCharacterList::get (NetworkMessage* msg)
{
        _nChars = new TWord8 (msg);
        uint32_t n = _nChars->getVal ();
        for (uint32_t i = 0; i < n; i ++) {
                _charlist.push_back (new TCharacter (msg));
        }
}

void TCharacterList::put (NetworkMessage* msg) const
{
        _nChars->put (msg);
        //for these functions to be const we must make a copy of the
        //char list before iterating through it. Its not like this will
        //be in a tight loop so we take this option

        CharList tmp = _charlist;
        for (CharList::iterator i = tmp.begin (); i != tmp.end (); ++ i) {
                (*i)->put (msg);
        }
}

void TCharacterList::show () const
{
        printf ("\t"); _nChars->show (); printf (" characters {\n");

        CharList tmp = _charlist;
        for (CharList::iterator i = tmp.begin (); i != tmp.end (); ++ i) {
                printf ("\t\t");
                (*i)->show ();
                printf ("\n");
        }
        printf ("\t}"); 
}

/******************************************************************
 * TPos
 ******************************************************************/

TPos::TPos (NetworkMessage* msg)
{
        get (msg);
}

TPos::TPos (uint16_t xPos, uint16_t yPos, uint8_t zPos)
{
        _x = new TWord16 (xPos);
        _y = new TWord16 (yPos);
        _z = new TWord8  (zPos);
}

TPos::TPos (const TPos& clone)
{
        _x = new TWord16 (*clone._x);
        _y = new TWord16 (*clone._y);
        _z = new TWord8  (*clone._z);
}

TPos::~TPos ()
{
        delete _x;
        delete _y;
        delete _z;
}

void TPos::put (NetworkMessage* msg) const
{
        _x->put (msg);
        _y->put (msg);
        _z->put (msg);
}

void TPos::show () const
{
        printf ("\tTPos {x: "); _x->show ();
        printf (" y: "); _y->show ();
        printf (" z: "); _z->show ();
        printf ("}\n");
}

uint16_t TPos::x () const
{
        return (_x->getVal ());
}

uint16_t TPos::y () const
{
        return (_y->getVal ());
}

uint8_t TPos::z () const
{
        return (_z->getVal ());
}

void TPos::get (NetworkMessage* msg)
{
        _x = new TWord16 (msg);
        _y = new TWord16 (msg);
        _z = new TWord8  (msg);
}

/******************************************************************
 * TItemLight
 ******************************************************************/

TItemLight::TItemLight (NetworkMessage* msg)
{
        get (msg);
}

TItemLight::TItemLight (uint16_t color, uint16_t radius)
{
         _color = new TWord16 (color);
         _radius = new TWord16 (radius);
}

TItemLight::TItemLight (const TItemLight& clone)
{
        _color = new TWord16 (*clone._color);
        _radius = new TWord16 (*clone._radius);
}

TItemLight::~TItemLight ()
{
        delete _color;
        delete _radius;
}

uint16_t TItemLight::getColor () const
{
        return (_color->getVal ());
}

uint16_t TItemLight::getRadius () const
{
        return (_radius->getVal ());
}

void TItemLight::put (NetworkMessage* msg) const
{
        _color->put (msg);
        _radius->put (msg);
}

void TItemLight::show () const
{
        printf ("\tTItemLight {color: "); _color->show ();
        printf (" raduis"); _radius->show (); printf ("}\n");
}

void TItemLight::get (NetworkMessage* msg)
{
        _color = new TWord16 (msg);
        _radius = new TWord16 (msg);
}

/******************************************************************
 * TCreatureLight
 ******************************************************************/

TCreatureLight::TCreatureLight (NetworkMessage* msg)
{
        get (msg);
}

TCreatureLight::TCreatureLight (uint8_t color, uint8_t radius)
{
         _color = new TWord8 (color);
         _radius = new TWord8 (radius);
}

TCreatureLight::TCreatureLight (const TCreatureLight& clone)
{
        _color = new TWord8 (*clone._color);
        _radius = new TWord8 (*clone._radius);
}

TCreatureLight::~TCreatureLight ()
{
        delete _color;
        delete _radius;
}

uint8_t TCreatureLight::getColor () const
{
        return (_color->getVal ());
}

uint8_t TCreatureLight::getRadius () const
{
        return (_radius->getVal ());
}

void TCreatureLight::put (NetworkMessage* msg) const
{
        _color->put (msg);
        _radius->put (msg);
}

void TCreatureLight::show () const
{
        printf ("\tTCreatureLight {color: "); _color->show ();
        printf (" raduis"); _radius->show (); printf ("}\n");
}

void TCreatureLight::get (NetworkMessage* msg)
{
        _color = new TWord8 (msg);
        _radius = new TWord8 (msg);
}

/**************************************************************
 * TThing - this is a long one
 **************************************************************/

//TItem

TItem::TItem (NetworkMessage* msg)
{
        get (msg);
}

TItem::TItem (uint16_t itemId)
{
        _itemId = new TWord16 (itemId);
}

TItem::TItem (const TItem& clone)
{
        _itemId = new TWord16 (*clone._itemId);
}

TItem::~TItem ()
{
        delete _itemId;
}

TItem::ThingType TItem::getType () const
{
        return TThing::item;
}
                
void TItem::show () const
{
        printf ("\tItem {id = "); _itemId->show (); printf ("}\n)");
}

void TItem::put (NetworkMessage* msg) const
{
        _itemId->put (msg);
}
        
uint16_t TItem::getItemId () const
{
        return _itemId->getVal ();
}

void TItem::get (NetworkMessage* msg)
{
        _itemId = new TWord16 (msg);
}

//TXItem

TXItem::TXItem (NetworkMessage* msg)
{
        get (msg);
}

TXItem::TXItem (uint16_t itemId, uint8_t xByte)
{
        _itemId = new TWord16 (itemId);
        _xByte = new TWord8 (xByte);
}

TXItem::TXItem (const TXItem& clone)
{
        _itemId = new TWord16 (*clone._itemId);
        _xByte = new TWord8 (*clone._xByte);
}

TXItem::~TXItem ()
{
        delete _itemId;
        delete _xByte;
}

TThing::ThingType TXItem::getType () const
{
        return TThing::xitem;
}
                
void TXItem::show () const
{
        printf ("\tXItem {id = "); _itemId->show (); 
        printf ("xByte = "); _xByte->show (); printf ("}\n)");
}

void TXItem::put (NetworkMessage* msg) const
{
        _itemId->put (msg);
        _xByte->put (msg);
}
        

uint16_t TXItem::getItemId () const
{
        return _itemId->getVal ();
}

uint8_t TXItem::getXByte () const
{
        return _xByte->getVal ();
}

void TXItem::get (NetworkMessage* msg)
{
        _itemId = new TWord16 (msg);
        _xByte = new TWord8 (msg);
}

//TCreature

TCreature::TCreature (NetworkMessage* msg, DatReader* dat)
{
        get (msg, dat);
}

TCreature::TCreature (uint32_t tibiaId, const std::string name, uint8_t hp,
                uint8_t direction, const TOutfit& outfit,
                const TCreatureLight& light, uint16_t speed,
                uint8_t skull, uint8_t shield)
{
        _tibiaId =      new TWord32 (tibiaId);
        _name =         new TString (name);
        _hp =           new TWord8 (hp);
        _direction =    new TWord8 (direction);
        _light =        new TCreatureLight (light);
        _speed =        new TWord16 (speed);
        _skull =        new TWord8 (skull);
        _shield =       new TWord8 (shield);
        
        TOutfitFactory of;
        _outfit =       of.cloneOutfit (outfit);
}

TCreature::TCreature (const TCreature& clone)
{
        _tibiaId =      new TWord32 (*clone._tibiaId);
        _name =         new TString (*clone._name);
        _hp =           new TWord8 (*clone._hp);
        _direction =    new TWord8 (*clone._direction);
        _light =        new TCreatureLight (*clone._light);
        _speed =        new TWord16 (*clone._speed);
        _skull =        new TWord8 (*clone._skull);
        _shield =       new TWord8 (*clone._shield);
        
        TOutfitFactory of;
        _outfit =       of.cloneOutfit (*clone._outfit);
}

TCreature::~TCreature ()
{
        delete _tibiaId;
        delete _name;
        delete _hp;
        delete _direction;
        delete _outfit;
        delete _light;
        delete _speed;
        delete _skull;
        delete _shield;
}

TThing::ThingType TCreature::getType () const
{
        return TThing::creature;
}
                
void TCreature::show () const
{
        printf ("\tTCreature {\n");
        printf ("\t\t_tibiaId: "); _tibiaId->show (); printf ("\n");
        printf ("\t\t_name: "); _name->show (); printf ("\n");
        printf ("\t\t_hp: "); _hp->show (); printf ("\n");
        printf ("\t\t_direction: "); _direction->show (); printf ("\n");
        printf ("\t\t_outfit: "); _outfit->show (); printf ("\n");
        printf ("\t\t_light: "); _light->show (); printf ("\n");
        printf ("\t\t_speed: "); _speed->show (); printf ("\n");
        printf ("\t\t_skull: "); _skull->show (); printf ("\n");
        printf ("\t\t_shield: "); _shield->show (); printf ("\n");
        printf ("}\n");
}

void TCreature::put (NetworkMessage* msg) const
{
        _tibiaId->put (msg);
        _name->put (msg);
        _hp->put (msg);
        _direction->put (msg);
        _outfit->put (msg);
        _light->put (msg);
        _speed->put (msg);
        _skull->put (msg);
        _shield->put (msg);
}

uint32_t TCreature::getTibiaId () const
{
        return _tibiaId->getVal ();
}

const std::string& TCreature::getName () const
{
        return _name->getString ();
}

uint8_t TCreature::getHp () const
{
        return _hp->getVal ();
}

uint8_t TCreature::getDirection () const
{
        return _direction->getVal ();
}

const TOutfit& TCreature::getOutfit () const
{
        return *_outfit;
}

const TCreatureLight& TCreature::getCreatureLight () const
{
        return *_light;
}

uint16_t TCreature::getSpeed () const
{
        return _speed->getVal ();
}

uint8_t TCreature::getSkull () const
{
        return _skull->getVal ();
}

uint8_t TCreature::getShield () const
{
        return _shield->getVal ();
}

void TCreature::get (NetworkMessage* msg, DatReader* dat)
{
        _tibiaId = new TWord32 (msg); 
        _name = new TString (msg); 
        _hp = new TWord8 (msg); 
        _direction = new TWord8 (msg); 

        TOutfitFactory of (msg, dat);
        _outfit = of.getOutfit ();

        _light = new TCreatureLight (msg); 
        _speed = new TWord16 (msg); 
        _skull = new TWord8 (msg); 
        _shield = new TWord8 (msg); 

}

//Yes this could be derived from TCreature, but that makes thing complicated
// TOldCreature

TOldCreature::TOldCreature (NetworkMessage* msg, DatReader* dat)
{
        get (msg, dat);
}

TOldCreature::TOldCreature (uint32_t tibiaId, const std::string name, 
                uint8_t hp, uint8_t direction, const TOutfit& outfit, 
                const TCreatureLight& light, uint16_t speed,
                uint8_t skull, uint8_t shield)
{
        _itemId = new TWord16 ((uint16_t)0x0062);
        _creature = new TCreature (tibiaId, name, hp, direction, outfit, light,
                                        speed, skull, shield);
}

TOldCreature::TOldCreature (const TOldCreature& clone)
{
        _itemId = new TWord16 (*clone._itemId);
        _creature = new TCreature (*clone._creature);
}

TOldCreature::~TOldCreature ()
{
        delete _itemId;
        delete _creature;
}

TThing::ThingType TOldCreature::getType () const
{
        return TThing::oldcreature;
}
                
void TOldCreature::show () const
{
        printf ("\tOldCreature {\n");
        _creature->show ();
        printf ("}\n");
}

void TOldCreature::put (NetworkMessage* msg) const
{
        _itemId->put (msg);
        _creature->put (msg);
}

TCreature* TOldCreature::getCreature () const
{
        return new TCreature (*_creature);
}

void TOldCreature::get (NetworkMessage* msg, DatReader* dat)
{
        _itemId = new TWord16 (msg);
        _creature = new TCreature (msg, dat);
};

// TNewCreature
TNewCreature::TNewCreature (NetworkMessage* msg, DatReader* dat)
{
        get (msg, dat);
}

TNewCreature::TNewCreature (uint32_t removeId, uint32_t tibiaId, 
                const std::string name, uint8_t hp, uint8_t direction, 
                const TOutfit& outfit, const TCreatureLight& light, 
                uint16_t speed, uint8_t skull, uint8_t shield)
{
        _itemId = new TWord16 ((uint16_t)0x0061);
        _removeId = new TWord32 (removeId);
        _creature = new TCreature (tibiaId, name, hp, direction, outfit, light,
                                        speed, skull, shield);
}

TNewCreature::TNewCreature (const TNewCreature& clone)
{
        _itemId = new TWord16 (*clone._itemId);
        _removeId = new TWord32 (*clone._removeId);
        _creature = new TCreature (*clone._creature);
}

TNewCreature::~TNewCreature ()
{
        delete _itemId;
        delete _removeId;
        delete _creature;
}

TThing::ThingType TNewCreature::getType () const
{
        return TThing::newcreature;
}
                
void TNewCreature::show () const
{
        printf ("\tNewCreature {removeId: "); _removeId->show (); printf ("\n");
        _creature->show ();
        printf ("}\n");
}

void TNewCreature::put (NetworkMessage* msg) const
{
        _itemId->put (msg);
        _removeId->put (msg);
        _creature->put (msg);
}

uint32_t TNewCreature::getRemoveId () const
{
        return _removeId->getVal () ;
}

TCreature* TNewCreature::getCreature () const
{
        return new TCreature (*_creature);
}

void TNewCreature::get (NetworkMessage* msg, DatReader* dat)
{
        _itemId = new TWord16 (msg);
        _removeId = new TWord32 (msg);
        _creature = new TCreature (msg, dat);
};

//TCreatureTurn
TCreatureTurn::TCreatureTurn (NetworkMessage* msg)
{
        get (msg);
}

TCreatureTurn::TCreatureTurn (uint32_t tibiaId, uint8_t dir)
{
        _itemId = new TWord16 ((uint16_t)0x0063);
        _tibiaId = new TWord32 (tibiaId);
        _dir = new TWord8 (dir);
}

TCreatureTurn::TCreatureTurn (const TCreatureTurn& clone)
{
        _itemId = new TWord16 (*clone._itemId);
        _tibiaId = new TWord32 (*clone._tibiaId);
        _dir = new TWord8 (*clone._dir);
}

TCreatureTurn::~TCreatureTurn ()
{
        delete _itemId;
        delete _tibiaId;
        delete _dir;
}

TThing::ThingType TCreatureTurn::getType () const
{
        return TThing::creatureturn;
}
                
void TCreatureTurn::show () const
{
        printf ("\tTCreatureTurn {tibiaId: "); _tibiaId->show ();
        printf (" dir: "); _dir->show (); printf ("}\n");
}

void TCreatureTurn::put (NetworkMessage* msg) const
{
        _itemId->put (msg);
        _tibiaId->put (msg);
        _dir->put (msg);
}

uint32_t TCreatureTurn::getTibiaId () const
{
        return _tibiaId->getVal ();
}

uint8_t TCreatureTurn::getDir () const
{
        return _dir->getVal ();
}

void TCreatureTurn::get (NetworkMessage* msg)
{
        _itemId = new TWord16 (msg);
        _tibiaId = new TWord32 (msg);
        _dir = new TWord8 (msg);
};

//TSkip
TSkip::TSkip (NetworkMessage* msg)
{
        get (msg);
}

TSkip::TSkip (uint8_t n)
{
        _ff = new TWord8 ((uint8_t)0xFF);
        _n = new TWord8 (n);
}

TSkip::TSkip (const TSkip& clone)
{
        _n = new TWord8 (*clone._n);
        _ff = new TWord8 (*clone._ff);
}

TSkip::~TSkip ()
{
        delete _ff;
        delete _n;
}

TThing::ThingType TSkip::getType () const
{
        return TThing::skip;
}

void TSkip::show () const
{
        printf ("\tTSkip {n: "); _n->show (); printf ("}\n");
}

void TSkip::put (NetworkMessage* msg) const
{
        _n->put (msg);
        _ff->put (msg);
}

uint8_t TSkip::getN () const
{
        return _n->getVal ();
}

void TSkip::get (NetworkMessage* msg)
{
        _n = new TWord8 (msg);
        _ff = new TWord8 (msg);
}

/***************************************************************************
 * TThingFactory
 ***************************************************************************/

TThingFactory::TThingFactory (NetworkMessage* msg, DatReader* dat)
{
        _msg = msg;
        _dat = dat;
        _readable = 1;
}

TThingFactory::TThingFactory ()
{
        _readable = 0;
}

TThing* TThingFactory::getThing ()
{
        if (!_readable) {
                printf ("error: TThingFactory not initialized to read\n");
                return NULL;
        }
        uint16_t id;
        _msg->peekU16 (id);

        if (id == 0x0061) {
                return new TNewCreature (_msg, _dat);
        } else if (id == 0x0062) {
                return new TOldCreature (_msg, _dat);
        } else if (id == 0x0063) {
                return new TCreatureTurn (_msg);
        } else if (id >= 0xFF00) {
                return new TSkip (_msg);
        } else {
                if (_dat->getItemData (id)->isXItem ()) {
                        return new TXItem (_msg);
                } else {
                        return new TItem (_msg);
                }
        }
}

TThing* TThingFactory::cloneThing (const TThing& thing)
{
        TThing::ThingType tt = thing.getType ();
        if (tt == TThing::item) {
                return new TItem ((const TItem&)thing);
        } else if (tt == TThing::xitem) {
                return new TXItem ((const TXItem&)thing);
        } else if (tt == TThing::creature) {
                return new TCreature ((const TCreature&)thing);
        } else if (tt == TThing::oldcreature) {
                return new TOldCreature ((const TOldCreature&)thing);
        } else if (tt == TThing::newcreature) {
                return new TNewCreature ((const TNewCreature&)thing);
        } else if (tt == TThing::skip) {
                return new TSkip ((const TSkip&)thing);
        } else {
                printf ("error: unreconized ThingType %d\n", tt);
                return NULL;
        }
}

/**************************************************************
 * TOutfit
 **************************************************************/

// TItemOutift
TItemOutfit::TItemOutfit (NetworkMessage* msg, DatReader* dat)
{
        get (msg, dat);
}

TItemOutfit::TItemOutfit (const TThing& item)
{
        _lookType = new TWord16 ((uint16_t)0x0000);
        TThingFactory tf;
        _item = tf.cloneThing (item);
}

TItemOutfit::TItemOutfit (const TItemOutfit& clone)
{
        _lookType = new TWord16 (*clone._lookType);
        TThingFactory tf;
        _item = tf.cloneThing (*clone._item);
}

TItemOutfit::~TItemOutfit ()
{
        delete _lookType;
        delete _item;
}

TOutfit::OutfitType TItemOutfit::getType () const
{
        return TOutfit::itemoutfit;
}

void TItemOutfit::put (NetworkMessage* msg) const
{
        _lookType->put (msg);
        _item->put (msg);
}

void TItemOutfit::show () const
{
        printf ("TItemOutfit {"); _item->show (); printf ("}\n");
}

void TItemOutfit::get (NetworkMessage* msg, DatReader* dat)
{
        _lookType = new TWord16 (msg);
        TThingFactory tf (msg, dat);
        _item = tf.getThing ();
}

//TCharOutfit

TCharOutfit::TCharOutfit (NetworkMessage* msg)
{
        get (msg);
}

TCharOutfit::TCharOutfit (uint16_t lookType, uint8_t head, uint8_t body,
                uint8_t legs, int8_t feet, uint8_t addons)
{
        _lookType = new TWord16 (lookType);
        _head = new TWord8 (head);
        _body = new TWord8 (body);
        _legs = new TWord8 (legs);
        _feet = new TWord8 (feet);
        _addons = new TWord8 (addons);
}

TCharOutfit::TCharOutfit (const TCharOutfit& clone)
{
        _lookType = new TWord16 (*clone._lookType);
        _head = new TWord8 (*clone._head);
        _body = new TWord8 (*clone._body);
        _legs = new TWord8 (*clone._legs);
        _feet = new TWord8 (*clone._feet);
        _addons = new TWord8 (*clone._addons);
}

TCharOutfit::~TCharOutfit ()
{
        delete _lookType;
        delete _head;
        delete _body;
        delete _legs;
        delete _feet;
        delete _addons;
}

TOutfit::OutfitType TCharOutfit::getType () const
{
        return TOutfit::charoutfit;
}

void TCharOutfit::put (NetworkMessage* msg) const
{
        _lookType->put (msg);
        _head->put (msg);
        _body->put (msg);
        _legs->put (msg);
        _feet->put (msg);
        _addons->put (msg);
}

void TCharOutfit::show () const
{
        printf ("\tTCharOutfit {\n");
        printf ("\t\tlookType: "); _lookType->show (); printf ("\n");
        printf ("\t\thead: "); _head->show (); printf ("\n");
        printf ("\t\tbody: "); _body->show (); printf ("\n");
        printf ("\t\tlegs: "); _legs->show (); printf ("\n");
        printf ("\t\tfeet: "); _feet->show (); printf ("\n");
        printf ("\t\taddons: "); _addons->show (); printf ("\n");
        printf ("}\n");
}

void TCharOutfit::get (NetworkMessage* msg)
{
        _lookType = new TWord16 (msg);
        _head = new TWord8 (msg);
        _body = new TWord8 (msg);
        _legs = new TWord8 (msg);
        _feet = new TWord8 (msg);
        _addons = new TWord8 (msg);
}

/********************************************************************
 * TOutfitFactory
 ********************************************************************/

TOutfitFactory::TOutfitFactory (NetworkMessage* msg, DatReader* dat)
{
        _msg = msg;
        _dat = dat;
        _readable = 1;
}

TOutfitFactory::TOutfitFactory ()
{
        _readable = 0;
}

TOutfit* TOutfitFactory::cloneOutfit (const TOutfit& clone)
{
        TOutfit::OutfitType ot = clone.getType ();
        if (ot == TOutfit::itemoutfit) {
                return new TItemOutfit((const TItemOutfit&)clone);
        } else if (ot == TOutfit::charoutfit) {
                return new TCharOutfit((const TCharOutfit&)clone);
        } else {
                printf ("error: unreconized outfit type %d\n", ot);
                return NULL;
        }
}
        
TOutfit* TOutfitFactory::getOutfit ()
{
        if (!_readable) {
                printf ("error: outfit factory is not initialized to read\n");
                return NULL;
        }
        uint16_t looktype;
        _msg->peekU16 (looktype);
        if (looktype == 0x0000) {
                return new TItemOutfit (_msg, _dat);
        } else {
                return new TCharOutfit (_msg);
        }
}

/************************************************************************
 * TMapDescription
 ************************************************************************/

TMapDescription::TMapDescription (const TPos& start, const TPos& end,
                NetworkMessage* msg, DatReader* dat)
{
        _start = new TPos (start);
        _cur = new TPos (start);
        _end = new TPos (end);
        _it = _map.begin ();

        get (start, end, msg, dat);
}
                
        
TMapDescription::TMapDescription (const TPos& start, const TPos& end)
{
        _start = new TPos (start);
        _cur = new TPos (start);
        _end = new TPos (end);
        _it = _map.begin ();
}

TMapDescription::TMapDescription (const TMapDescription& clone)
{
        _start = new TPos (*clone._start);
        _cur = new TPos (*clone._start);
        _end = new TPos (*clone._end);
        //use _it to go through clone list, then well assign
        //it afterwards
        TThingFactory tf;

        MapList::const_iterator i;
        for (i = clone._map.begin (); i != clone._map.end (); ++i) {
                _map.push_back (tf.cloneThing (*(*i)));
        }
        _it = _map.begin ();
}

TMapDescription::~TMapDescription ()
{
        delete _start;
        delete _cur;
        delete _end;

        for (_it = _map.begin (); _it != _map.end (); ++_it) {
                delete (*_it);
        }
}

void TMapDescription::put (NetworkMessage* msg) const
{
        MapList::const_iterator i;
        for (i = _map.begin (); i != _map.end (); ++i) {
                (*i)->put (msg);
        }
}

void TMapDescription::show () const
{
        printf ("\tTMapDescription {\n");
        MapList::const_iterator i;
        for (i = _map.begin (); i != _map.end (); ++i) {
                printf ("\t"); (*i)->show (); printf ("\n");
        }
        printf ("}\n");
}
        
const TPos& TMapDescription::getStart () const
{
        return *_start;
}

const TPos& TMapDescription::getEnd () const
{
        return *_end;
}


void TMapDescription::begin ()
{
        _it = _map.begin ();
}

bool TMapDescription::isEnd ()
{
        if (_it == _map.end ()) {
                return true;
        } else {
                return false;
        }
}

void TMapDescription::next ()
{
        if (_it == _map.end ()) {
                printf ("error: attempt to seek past end of map\n");
        } else {
                _it ++;
        }
}

const TThing& TMapDescription::getThing ()
{
        return *(*_it);
}

void TMapDescription::insert (TThing* tm)
{
        _map.insert (_it, tm);
}

void TMapDescription::replace (TThing* tm)
{
        if (_map.size () == 0) {
                printf ("map error: attemp to replace in empty list\n");
                return;
        }
        if (_it == _map.end ()) {
                printf ("map error: attemp to replace \"end\"\n");
                return;
        }
        _it = _map.erase (_it);
        _map.insert (_it, tm);
        _it --;
} 

void TMapDescription::remove ()
{
        _it = _map.erase (_it);
}

void TMapDescription::add (TThing* thing)
{
        _map.push_back (thing);
}

void TMapDescription::get (const TPos& start, const TPos& end,
                NetworkMessage* msg, DatReader* dat)
{
        uint32_t nThings = (end.x () - start.x () + 1) *
                         (end.y () - start.y () + 1) *
                         (end.z () - start.z () + 1);

        TThingFactory tf (msg, dat);
        TThing* thing;
        while (nThings != 0) {
                thing = tf.getThing ();
                if (thing->getType () == TThing::skip) {
                        //we alway skip one byte
                        nThings --;
                        nThings -= ((TSkip*)thing)->getN ();
                }
                _map.push_back (thing);
        }
}

/*************************************************************************
 * TEffect
 *************************************************************************/

TEffect::TEffect (NetworkMessage* msg)
{
        get (msg);
}

TEffect::TEffect (const TPos& pos, uint8_t effectId)
{
        _pos = new TPos (pos);
        _effectId = new TWord8 (effectId);
}

TEffect::TEffect (const TEffect& clone)
{
        _pos = new TPos (*clone._pos);
        _effectId = new TWord8 (*clone._effectId);
}
        
TEffect::~TEffect ()
{
        delete _pos;
        delete _effectId;
}

void TEffect::put (NetworkMessage* msg) const
{
        _pos->put (msg);
        _effectId->put (msg);
}

void TEffect::show () const
{
        printf ("TPos {"); _pos->show ();
        printf (", Effect:"); _effectId->show();
        printf ("}");
}

const TPos& TEffect::getPos () const
{
        return *_pos;
}

uint8_t TEffect::getEffectId () const
{
        return _effectId->getVal ();
}

void TEffect::get (NetworkMessage* msg)
{
        _pos = new TPos (msg);
        _effectId = new TWord8 (msg);
}

/*************************************************************************
 * TTextMsg
 *************************************************************************/

TTextMsg::TTextMsg (NetworkMessage* msg)
{
        get (msg);
}

TTextMsg::TTextMsg (uint8_t msgType, const std::string msg)
{
        _msgType = new TWord8 (msgType);
        _msg = new TString (msg);
}

TTextMsg::TTextMsg (const TTextMsg& clone)
{
        _msgType = new TWord8 (*clone._msgType);
        _msg = new TString (*clone._msg);
}

TTextMsg::~TTextMsg ()
{
        delete _msgType;
        delete _msg;
}

void TTextMsg::put (NetworkMessage* msg) const
{
        _msgType->put (msg);
        _msg->put (msg);
}

void TTextMsg::show () const
{
        printf ("TTextMsg {type: "); _msgType->show ();
        printf ("msg: "); _msg->show ();
        printf ("}\n");
}

uint8_t TTextMsg::getMsgType () const
{
        return _msgType->getVal ();
}

const std::string TTextMsg::getMsg () const
{
        return _msg->getString ();
}

void TTextMsg::get (NetworkMessage* msg)
{
        _msgType = new TWord8 (msg);
        _msg = new TString (msg);
}

/*************************************************************************
 * TPlayerStats
 *************************************************************************/

TPlayerStats::TPlayerStats (NetworkMessage* msg)
{
        get (msg);
}

TPlayerStats::TPlayerStats (uint16_t hp,
                        uint16_t hpmax,
                        uint16_t capacity,
                        uint32_t experience,
                        uint16_t level,
                        uint8_t levelPercent,
                        uint16_t mana,
                        uint16_t maxmana,
                        uint8_t magicLevel,
                        uint8_t magicLevelPercent,
                        uint8_t soul,
                        uint16_t stamina)
{
        _hp =                  new TWord16 (hp);
        _hpmax =               new TWord16 (hpmax);
        _capacity =            new TWord16 (capacity);
        _experience =          new TWord32 (experience);
        _level =               new TWord16 (level);
        _levelPercent =        new TWord8 (levelPercent);
        _mana =                new TWord16 (mana);
        _maxmana =             new TWord16 (maxmana);
        _magicLevel =          new TWord8 (magicLevel);
        _magicLevelPercent =   new TWord8 (magicLevelPercent);
        _soul =                new TWord8 (soul);
        _stamina =             new TWord16 (stamina);
}

TPlayerStats::TPlayerStats (const TPlayerStats& clone)
{
        _hp =                  new TWord16 (*clone._hp);
        _hpmax =               new TWord16 (*clone._hpmax);
        _capacity =            new TWord16 (*clone._capacity);
        _experience =          new TWord32 (*clone._experience);
        _level =               new TWord16 (*clone._level);
        _levelPercent =        new TWord8 (*clone._levelPercent);
        _mana =                new TWord16 (*clone._mana);
        _maxmana =             new TWord16 (*clone._maxmana);
        _magicLevel =          new TWord8 (*clone._magicLevel);
        _magicLevelPercent =   new TWord8 (*clone._magicLevelPercent);
        _soul =                new TWord8 (*clone._soul);
        _stamina =             new TWord16 (*clone._stamina);
}

TPlayerStats::~TPlayerStats ()
{
        delete _hp;
        delete _hpmax;
        delete _capacity;
        delete _experience;
        delete _level;
        delete _levelPercent;
        delete _mana;
        delete _maxmana;
        delete _magicLevel;
        delete _magicLevelPercent;
        delete _soul;
        delete _stamina;
}

void TPlayerStats::put (NetworkMessage* msg)
{
        _hp->put (msg);
        _hpmax->put (msg);
        _capacity->put (msg);
        _experience->put (msg);
        _level->put (msg);
        _levelPercent->put (msg);
        _mana->put (msg);
        _maxmana->put (msg);
        _magicLevel->put (msg);
        _magicLevelPercent->put (msg);
        _soul->put (msg);
        _stamina->put (msg);
}

void TPlayerStats::show ()
{
        printf ("TPlayerStats {\n");
        printf ("\t\thp: "); _hp->show (); printf ("\n");
        printf ("\t\thpmax: "); _hpmax->show (); printf ("\n");
        printf ("\t\tcapacity: "); _capacity->show (); printf ("\n");
        printf ("\t\texperience: "); _experience->show (); printf ("\n");
        printf ("\t\tlevel: "); _level->show (); printf ("\n");
        printf ("\t\tlevelPercent: "); _levelPercent->show (); printf ("\n");
        printf ("\t\tmana: "); _mana->show (); printf ("\n");
        printf ("\t\tmaxmana: "); _maxmana->show (); printf ("\n");
        printf ("\t\tmagicLevel: "); _magicLevel->show (); printf ("\n");
        printf ("\t\tmagicLevelPercent: "); _magicLevelPercent->show (); printf ("\n");
        printf ("\t\tsoul: "); _soul->show (); printf ("\n");
        printf ("\t\tstamina: "); _stamina->show (); printf ("\n");
        printf ("}\n");
}

uint16_t TPlayerStats::getHp () const
{
        return _hp->getVal ();
}

uint16_t TPlayerStats::getHpmax () const
{
        return _hpmax->getVal ();
}

uint16_t TPlayerStats::getCapacity () const
{
        return _capacity->getVal ();
}

uint32_t TPlayerStats::getExperience () const
{
        return _experience->getVal ();
}

uint16_t TPlayerStats::getLevel () const
{
        return _level->getVal ();
}

uint8_t TPlayerStats::getLevelPercent () const
{
        return _levelPercent->getVal ();
}

uint16_t TPlayerStats::getMana () const
{
        return _mana->getVal ();
}

uint16_t TPlayerStats::getMaxmana () const
{
        return _maxmana->getVal ();
}

uint8_t TPlayerStats::getMagicLevel () const
{
        return _magicLevel->getVal ();
}

uint8_t TPlayerStats::getMagicLevelPercent () const
{
        return _magicLevelPercent->getVal ();
}

uint8_t TPlayerStats::getSoul () const
{
        return _soul->getVal ();
}

uint16_t TPlayerStats::getStamina () const
{
        return _stamina->getVal ();
}

void TPlayerStats::get (NetworkMessage* msg)
{
        _hp =                  new TWord16 (msg);
        _hpmax =               new TWord16 (msg);
        _capacity =            new TWord16 (msg);
        _experience =          new TWord32 (msg);
        _level =               new TWord16 (msg);
        _levelPercent =        new TWord8 (msg);
        _mana =                new TWord16 (msg);
        _maxmana =             new TWord16 (msg);
        _magicLevel =          new TWord8 (msg);
        _magicLevelPercent =   new TWord8 (msg);
        _soul =                new TWord8 (msg);
        _stamina =             new TWord16 (msg);
} 

/*************************************************************************
 * TPlayerSkill
 *************************************************************************/

TPlayerSkill::TPlayerSkill (NetworkMessage* msg)
{
        get (msg);
}

TPlayerSkill::TPlayerSkill (uint8_t level, uint8_t percent)
{
        _level = new TWord8 (level);
        _percent = new TWord8 (percent);
}

TPlayerSkill::TPlayerSkill (const TPlayerSkill& clone)
{
        _level = new TWord8 (*clone._level);
        _percent = new TWord8 (*clone._percent);
}

TPlayerSkill::~TPlayerSkill ()
{
        delete _level;
        delete _percent;
}

void TPlayerSkill::put (NetworkMessage* msg) const
{
        _level->put (msg);
        _percent->put (msg);
}

void TPlayerSkill::show () const
{
        printf ("TPlayerSkill {lvl: "); _level->show ();
        printf ("\%: "); _percent->show (); printf ("}");
}
        
uint8_t TPlayerSkill::getLevel () const
{
        return _level->getVal ();
}

uint8_t TPlayerSkill::getPercent () const
{
        return _percent->getVal ();
}

void TPlayerSkill::get (NetworkMessage* msg)
{
        _level = new TWord8 (msg);
        _percent = new TWord8 (msg);
}

/*************************************************************************
 * TPlayerSkills
 *************************************************************************/

TPlayerSkills::TPlayerSkills (NetworkMessage* msg)
{
        get (msg);
}

TPlayerSkills::TPlayerSkills ( const TPlayerSkill& fist,
                        const TPlayerSkill& club,
                        const TPlayerSkill& sword,
                        const TPlayerSkill& axe,
                        const TPlayerSkill& distance,
                        const TPlayerSkill& shield,
                        const TPlayerSkill& fishing)
{
        _fist =         new TPlayerSkill (fist);
        _club =         new TPlayerSkill (club);
        _sword =        new TPlayerSkill (sword);
        _axe =          new TPlayerSkill (axe);
        _distance =     new TPlayerSkill (distance);
        _shield =       new TPlayerSkill (shield);
        _fishing =      new TPlayerSkill (fishing);
}

TPlayerSkills::TPlayerSkills (const TPlayerSkills& clone)
{
        _fist =         new TPlayerSkill (*clone._fist);
        _club =         new TPlayerSkill (*clone._club);
        _sword =        new TPlayerSkill (*clone._sword);
        _axe =          new TPlayerSkill (*clone._axe);
        _distance =     new TPlayerSkill (*clone._distance);
        _shield =       new TPlayerSkill (*clone._shield);
        _fishing =      new TPlayerSkill (*clone._fishing);
}

TPlayerSkills::~TPlayerSkills ()
{
        delete _fist;
        delete _club;
        delete _sword;
        delete _axe;
        delete _distance;
        delete _shield;
        delete _fishing;
}

void TPlayerSkills::put (NetworkMessage* msg) const
{
        _fist->put (msg);
        _club->put (msg);
        _sword->put (msg);
        _axe->put (msg);
        _distance->put (msg);
        _shield->put (msg);
        _fishing->put (msg);
}

void TPlayerSkills::show () const
{
        printf ("TPlayerSkills {\n");
        printf ("\t\tfist: "); _fist->show (); printf ("\n");
        printf ("\t\tclub: "); _club->show (); printf ("\n");
        printf ("\t\tsword: "); _sword->show (); printf ("\n");
        printf ("\t\taxe: "); _axe->show (); printf ("\n");
        printf ("\t\tdistance: "); _distance->show (); printf ("\n");
        printf ("\t\tshield: "); _shield->show (); printf ("\n");
        printf ("\t\tfishing: "); _fishing->show (); printf ("\n");
}

const TPlayerSkill& TPlayerSkills::getFist () const
{
        return *_fist;
}

const TPlayerSkill& TPlayerSkills::getClub () const
{
        return *_club;
}

const TPlayerSkill& TPlayerSkills::getSword () const
{
        return *_sword;
}

const TPlayerSkill& TPlayerSkills::getAxe () const
{
        return *_axe;
}

const TPlayerSkill& TPlayerSkills::getDistance () const
{
        return *_distance;
}

const TPlayerSkill& TPlayerSkills::getShield () const
{
        return *_shield;
}

const TPlayerSkill& TPlayerSkills::getFishing () const
{
        return *_fishing;
}

void TPlayerSkills::get (NetworkMessage* msg)
{
        _fist = new TPlayerSkill (msg);
        _club = new TPlayerSkill (msg);
        _sword = new TPlayerSkill (msg);
        _axe = new TPlayerSkill (msg);
        _distance = new TPlayerSkill (msg);
        _shield = new TPlayerSkill (msg);
        _fishing = new TPlayerSkill (msg);
}

/************************************************************************
 * TContainer
 ************************************************************************/

TContainer::TContainer (NetworkMessage* msg, DatReader* dat)
{
        get (msg, dat);
        _it = _items.begin ();
}


TContainer::TContainer (const TThing& item, const std::string& name,
                        uint8_t capacity, uint8_t hasParent)
{
        
        TThingFactory tf;
        _item = tf.cloneThing (item);
        _name = new TString (name);
        _capacity = new TWord8 (capacity);
        _hasParent = new TWord8 (hasParent);
        _nItems = new TWord8 ((uint8_t)0);
        _it = _items.begin ();
}
        
TContainer::TContainer (const TContainer& clone)
{
        TThingFactory tf;
        _item = tf.cloneThing (*clone._item);

        _name = new TString (*clone._name);
        _capacity = new TWord8 (*clone._capacity);
        _hasParent = new TWord8 (*clone._hasParent);
        _nItems = new TWord8 (*clone._nItems);

        ContainerList::const_iterator i;
        for (i = clone._items.begin (); i != clone._items.end (); ++ i) {
                _items.push_back (tf.cloneThing (**i));
        }

        _it = _items.begin ();
}
        
TContainer::~TContainer ()
{
        delete _item;
        delete _name;
        delete _capacity;
        delete _hasParent;
        delete _nItems;

        for (_it = _items.begin (); _it != _items.end (); ++ _it) {
                delete (*_it);
        }
}
        

void TContainer::put (NetworkMessage* msg) const
{
        _item->put (msg);
        _name->put (msg);
        _capacity->put (msg);
        _hasParent->put (msg);
        _nItems->put (msg);

        ContainerList::const_iterator i;
        for (i = _items.begin (); i != _items.end (); ++ i) {
                (*i)->put (msg);
        }
}
        
void TContainer::show () const
{
        printf ("\tTContainer {\n");
        printf ("\t\titem: "); _item->show (); printf ("\n");
        printf ("\t\tname: "); _name->show (); printf ("\n");
        printf ("\t\tcapacity: "); _capacity->show (); printf ("\n");
        printf ("\t\thasParent: "); _hasParent->show (); printf ("\n");
        printf ("\t\tnItems: "); _nItems->show (); printf ("\n");

        printf ("\t\tItems:\n");

        ContainerList::const_iterator i;
        for (i = _items.begin (); i != _items.end (); ++ i) {
                printf ("\t\t"); (*i)->show (); printf ("\n");
        }
        printf ("\t}\n");
}

const TThing& TContainer::getItem () const
{
        return *_item;
}

const std::string& TContainer::getName () const
{
        return _name->getString () ;
}

uint8_t TContainer::getCapacity () const
{
        return _capacity->getVal ();
}

uint8_t TContainer::getHasParent () const
{
        return _hasParent->getVal ();
}

uint8_t TContainer::getNItems () const
{
        return _nItems->getVal ();
}

void TContainer::begin ()
{
        _it = _items.begin ();
}

bool TContainer::isEnd ()
{
        if (_it == _items.end ()) {
                return true;
        } else {
                return false;
        }
}

void TContainer::next ()
{
        if (_it == _items.end ()) {
                printf ("error: attempt to seek past end of map\n");
        } else {
                _it ++;
        }
}

const TThing& TContainer::getThing ()
{
        return *(*_it);
}

void TContainer::insert (TThing* tm)
{
        _items.insert (_it, tm);
        uint8_t tmp = _nItems->getVal ();
        delete _nItems;
        _nItems = new TWord8 (tmp + 1);
}

void TContainer::replace (TThing* tm)
{
        if (_items.size () == 0) {
                printf ("map error: attemp to replace in empty list\n");
                return;
        }
        if (_it == _items.end ()) {
                printf ("map error: attemp to replace \"end\"\n");
                return;
        }
        _it = _items.erase (_it);
        _items.insert (_it, tm);
        _it --;
} 

void TContainer::remove ()
{
        _it = _items.erase (_it);
        uint8_t tmp = _nItems->getVal ();
        delete _nItems;
        _nItems = new TWord8 (tmp - 1);
}

void TContainer::add (TThing* thing)
{
        _items.push_back (thing);
        uint8_t tmp = _nItems->getVal ();
        delete _nItems;
        _nItems = new TWord8 (tmp + 1);
}

void TContainer::get (NetworkMessage* msg, DatReader* dat)
{
        TThingFactory tf (msg, dat);
        _item = tf.getThing ();

        _name = new TString (msg);
        _capacity = new TWord8 (msg);
        _hasParent = new TWord8 (msg);
        _nItems = new TWord8 (msg);

        uint32_t n = _nItems->getVal ();

        for (uint32_t i = 0; i < n; i ++) {
                _items.push_back (tf.getThing ());
        }

        _it = _items.begin ();
}
