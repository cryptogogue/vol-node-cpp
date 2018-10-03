// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEPTR_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLEPTR_H

#include <volition/common.h>

namespace Volition {

class AbstractSerializable;

//================================================================//
// AbstractSerializableTypeInfo
//================================================================//
class AbstractSerializableTypeInfo {
protected:

    //----------------------------------------------------------------//
    virtual string      AbstractSerializableTypeInfo_get        ( string key ) const = 0;

public:

    //----------------------------------------------------------------//
    string get ( string key ) const {
        return this->AbstractSerializableTypeInfo_get ( key );
    }
};

//================================================================//
// AbstractSerializablePointer
//================================================================//
class AbstractSerializablePointer {
public:

    //----------------------------------------------------------------//
    virtual AbstractSerializable*           AbstractSerializablePointer_get         () = 0;
    virtual const AbstractSerializable*     AbstractSerializablePointer_get         () const = 0;
    virtual void                            AbstractSerializablePointer_make        ( const AbstractSerializableTypeInfo& typeInfo ) = 0;
};

} // namespace Volition
#endif
