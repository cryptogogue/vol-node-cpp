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
        *this->mStream << value;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const double& value ) override {
        *this->mStream << value;
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
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const Variant& value ) override {
        
        switch ( value.mType ) {
            
            case Variant::TYPE_BOOL:
                *this->mStream << ( bool )( value.mNumeric == 1 );
                break;
            
            case Variant::TYPE_NUMBER:
                *this->mStream << value.mNumeric;
                break;
            
            case Variant::TYPE_STRING:
                *this->mStream << value.mString;
                break;
                
            default:
                break;
        }
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
