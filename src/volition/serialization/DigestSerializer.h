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
    void AbstractSerializerTo_affirmArray () override {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_affirmObject () override {
    }

    //----------------------------------------------------------------//
    SerializerPropertyName AbstractSerializerTo_getName () const override {
        return SerializerPropertyName ();
    }
    
    //----------------------------------------------------------------//
    AbstractSerializerTo* AbstractSerializerTo_getParent () override {
        return this;
    }

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () const override {
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const bool& value ) override {
        UNUSED ( name );
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const double& value ) override {
        UNUSED ( name );
        *this->mStream << value;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const u64& value ) override {
        UNUSED ( name );
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const string& value ) override {
        UNUSED ( name );
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializable& value ) override {
        UNUSED ( name );
        value.serialize ( *this );
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) override {
        UNUSED ( name );
        serializeFunc ( *this );
    }

    //----------------------------------------------------------------//
    void AbstractSerializerFrom_stringToTree ( SerializerPropertyName name, string value ) override {
        UNUSED ( name );
        UNUSED ( value );
        assert ( false );
    }

public:

    //----------------------------------------------------------------//
    static void hash ( const AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        DigestSerializer serializer;
        serializer.mStream = &digestStream;
        serializable.serialize ( serializer );
    }
};

} // namespace Volition
#endif
