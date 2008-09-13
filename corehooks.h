#ifndef __CORE_HOOKS_H
#define __CORE_HOOKS_H

#include <stdint.h>
#include "hook.h"

//hook naming should be HW for a write hook and HR for a read hook,
//followed by the message name

class HWCharacterList : public WriteHook
{
        public:
                virtual TibiaMessage* func (TibiaMessage* tm, Client* client);
};

class HWMOTD : public WriteHook
{
        public:
                virtual TibiaMessage* func (TibiaMessage* tm, Client* client);
};

class HRLoginMsg : public ReadHook
{
        public:
                virtual void func (TibiaMessage* tm, Client* client);
};

class HRGameInit : public ReadHook
{
        public:
                virtual void func (TibiaMessage* tm, Client* client);
};


/**********************************************************************
 * Protocol hooks
 **********************************************************************/


/**********************************************************************
 * Game Recv 
 **********************************************************************/

class GRHSelfInfo : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHSlotItem : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHMagicEffect : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHTextMsg : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHGlobalLight : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHCreatureLight : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHPlayerSkills : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

class GRHMapInit : public ReadHook
{
        virtual void func (TibiaMessage* tm, Client* client);
};

#endif
