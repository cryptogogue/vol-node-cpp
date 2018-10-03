// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_DIGESTSERIALIZER_H
#define VOLITION_SERIALIZATION_DIGESTSERIALIZER_H

#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// DigestSerializer
//================================================================//
class DigestSerializer :
    public AbstractSerializerTo {
private:

    Poco::DigestOutputStream*       mStream;

protected:

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () override {
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const u64& value ) override {
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const string& value ) override {
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializable& value ) override {
        value.serializeTo ( *this );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializableArray& value ) override {
        value.serializeTo ( *this );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializablePointer& value ) override {
    
        const AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            serializable->serializeTo ( *this );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractStringifiable& value ) override {
        *this->mStream << value.toString ();
    }

public:

    //----------------------------------------------------------------//
    static void hash ( const AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        DigestSerializer serializer;
        serializer.mStream = &digestStream;
        serializable.serializeTo ( serializer );
    }
};

} // namespace Volition
#endif
