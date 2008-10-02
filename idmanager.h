#ifndef ID_MANAGER_H__
#define ID_MANAGER_H__

/********************************************************************
 * IdManager
 * IdManager is used for managing unique ids
 ********************************************************************/

#include <stdint.h>
#include <set>
#include <list>

class IdManager
{
        public:
                IdManager ();
                IdManager (uint32_t bin_size);

                /* request a new id */
                uint32_t newId ();
                /* recycle an id, will return false if id not used */
                bool     recycleId (uint32_t id);
        private:
                uint32_t _bin_size;
                std::set<uint32_t> used;
                std::list<uint32_t> recyclebin;
};

#endif
                