// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSERIALIZABLE_H
#define VOLITION_ABSTRACTSERIALIZABLE_H

#include "common.h"

namespace Volition {

//================================================================//
// AbstractSerializable
//================================================================//
class AbstractSerializable {
private:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) = 0;
    virtual void        AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractSerializable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractSerializable () {
    }

    //----------------------------------------------------------------//
    void fromJSON ( const Poco::JSON::Object& object ) {
        this->AbstractSerializable_fromJSON ( object );
    }
    
    //----------------------------------------------------------------//
    void toJSON ( Poco::JSON::Object& object ) const {
        this->AbstractSerializable_toJSON ( object );
    }
};

} // namespace Volition
#endif
