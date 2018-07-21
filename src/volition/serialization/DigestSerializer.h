// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_DIGESTSERIALIZER_H
#define VOLITION_SERIALIZATION_DIGESTSERIALIZER_H

#include <volition/serialization/AbstractSerializer.h>

namespace Volition {

class AbstractSerializer;

//================================================================//
// DigestSerializer
//================================================================//
class DigestSerializer :
    public AbstractSerializer {
private:

    Poco::DigestOutputStream*       mStream;

protected:

    //----------------------------------------------------------------//
    Mode AbstractSerializer_getMode () override {
        return SERIALIZE_DIGEST;
    }
    
    //----------------------------------------------------------------//
    size_t AbstractSerializer_getSize () override {
        return 0;
    }

    //----------------------------------------------------------------//
    bool AbstractSerializer_has ( SerializerPropertyName name ) override {
        return false;
    }

    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, u64& value ) override {
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, string& value ) override {
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializable& value ) override {
        value.serialize ( *this );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializableArray& value ) override {
        value.serialize ( *this );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializablePointer& value ) override {
    
        AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            serializable->serialize ( *this );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, Poco::DigestEngine::Digest& value ) override {
        *this->mStream <<  Poco::DigestEngine::digestToHex ( value );
    }

public:

    //----------------------------------------------------------------//
    static void hash ( AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        DigestSerializer serializer;
        serializer.mStream = &digestStream;
        serializable.serialize ( serializer );
    }
};

} // namespace Volition
#endif
