#ifndef VOLITION_SINGLETON_H
#define VOLITION_SINGLETON_H

#include "common.h"

namespace Volition {

//================================================================//
// Singleton
//================================================================//
template < typename TYPE >
class Singleton {
public:

    //----------------------------------------------------------------//
    static TYPE& get () {
        static Poco::SingletonHolder < TYPE > single;
        return *single.get ();
    }
};

} // namespace Volition
#endif
