// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTHASHABLE_H
#define VOLITION_ABSTRACTHASHABLE_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractHashable
//================================================================//
class AbstractHashable {
private:

    //----------------------------------------------------------------//
    virtual void            AbstractHashable_hash            ( Poco::DigestOutputStream& digestStream ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractHashable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractHashable () {
    }

    //----------------------------------------------------------------//
    void hash ( Poco::DigestOutputStream& digestStream ) const {
        this->AbstractHashable_hash ( digestStream );
    }
};

} // namespace Volition
#endif
