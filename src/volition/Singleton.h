// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SINGLETON_H
#define VOLITION_SINGLETON_H

#include <volition/common.h>

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
