#ifdef WIN32
        /* do windows stuff here */
#else
        #include <dlfcn.h>
#endif

#include "plugin.h"

Plugin::Plugin ()
{
        _recipricantId = 0;
}

Plugin::~Plugin ()
{
        unload ();
}

bool Plugin::load (uint32_t pluginId, const std::string& path, Client* client)
{
#ifdef WIN32
        /* do windows stuff here */
#else
        if ((_handle = dlopen (path.c_str (), RTLD_NOW)) == NULL) {
                
                printf ("plugin error: %s\n", dlerror());
                return false;
        }
        if ((_load = (loadfunc) dlsym (_handle, "load")) == NULL) {
                printf ("plugin error: could not resolve symbol \"load\"\n");
                return false;
        }
        if ((_unload = (unloadfunc) dlsym (_handle, "unload")) == NULL) {
                printf ("plugin error: could not resolve symbol \"unload\"\n");
                return false;
        }
        if ((_name = (namefunc) dlsym (_handle, "name")) == NULL) {
                printf ("plugin error: could not resolve symbol \"name\"\n");
                return false;
        }
        _load (pluginId, client);
        _client = client;
        return true;
#endif
}
        
bool Plugin::unload ()
{
        _unload ();

        if (dlclose (_handle) != 0) {
                printf ("plugin error: %s\n", dlerror());
                return false;
        }
        return true;
}

const std::string& Plugin::name ()
{
        return _name ();
}

uint32_t Plugin::getRecipricantId ()
{
        return _recipricantId;
}

void Plugin::setRecipricantId (uint32_t rid)
{
        _recipricantId = rid;
}

void Plugin::addRecvReadHookId (uint32_t hid)
{
        std::pair <std::set<uint32_t>::iterator, bool> ret;
        ret = _rrhooks.insert (hid);
        if (!ret.second) {
                printf ("plugin error: addRecvReadHookId: ");
                printf ("hook id already exists\n");
        }
}

void Plugin::addRecvWriteHookId (uint32_t hid)
{
        std::pair <std::set<uint32_t>::iterator, bool> ret;
        ret = _rwhooks.insert (hid);
        if (!ret.second) {
                printf ("plugin error: addRecvWriteHookId: ");
                printf ("hook id already exists\n");
        }
}

void Plugin::addSendReadHookId (uint32_t hid)
{
        std::pair <std::set<uint32_t>::iterator, bool> ret;
        ret = _srhooks.insert (hid);
        if (!ret.second) {
                printf ("plugin error: addSendReadHookId: ");
                printf ("hook id already exists\n");
        }
}

void Plugin::addSendWriteHookId (uint32_t hid)
{
        std::pair <std::set<uint32_t>::iterator, bool> ret;
        ret = _swhooks.insert (hid);
        if (!ret.second) {
                printf ("plugin error: addSendWriteHookId: ");
                printf ("hook id already exists\n");
        }
}

void Plugin::deleteRecvReadHookId (uint32_t hid)
{
        if (_rrhooks.erase (hid) == 0) {
                printf ("plugin error: deleteRecvReadId: ");
                printf ("non existant hook id\n");
        }
}

void Plugin::deleteRecvWriteHookId (uint32_t hid)
{
        if (_rrhooks.erase (hid) == 0) {
                printf ("plugin error: deleteRecvWriteId: ");
                printf ("non existant hook id\n");
        }
}

void Plugin::deleteSendReadHookId (uint32_t hid)
{
        if (_rrhooks.erase (hid) == 0) {
                printf ("plugin error: deleteSendReadId: ");
                printf ("non existant hook id\n");
        }
}

void Plugin::deleteSendWriteHookId (uint32_t hid)
{
        if (_rrhooks.erase (hid) == 0) {
                printf ("plugin error: deleteSendWriteId: ");
                printf ("non existant hook id\n");
        }
}

bool Plugin::addConnectionId (uint32_t cid)
{
        std::pair <std::set<uint32_t>::iterator, bool> ret;
        ret = _connections.insert (cid);
        return ret.second;
}

bool Plugin::removeConnectionId (uint32_t hid)
{
        if (_connections.erase (hid) == 0) {
                return false;
        }
        return true;
}

