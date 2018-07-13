// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLE_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLE_H

#include <volition/common.h>

namespace Volition {

class AbstractSerializer;

//================================================================//
// AbstractSerializable
//================================================================//
class AbstractSerializable {
protected:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializable_serialize      ( AbstractSerializer& serializer ) = 0;

public:

    //----------------------------------------------------------------//
    AbstractSerializable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractSerializable () {
    }

    //----------------------------------------------------------------//
    void serialize ( AbstractSerializer& serializer ) {
        this->AbstractSerializable_serialize ( serializer );
    }
};

//================================================================//
// AbstractSerializableArray
//================================================================//
class AbstractSerializableArray :
    public AbstractSerializable {
public:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializableArray_resize        ( size_t size ) = 0;
};

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
    virtual AbstractSerializable*       AbstractSerializablePointer_get         () = 0;
    virtual void                        AbstractSerializablePointer_make        ( const AbstractSerializableTypeInfo& typeInfo ) = 0;
};

} // namespace Volition
#endif
