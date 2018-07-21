// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_TOJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_TOJSONSERIALIZER_H

#include <volition/serialization/JSONSerializer.h>

namespace Volition {

class AbstractSerializer;

//================================================================//
// ToJSONSerializer
//================================================================//
class ToJSONSerializer :
    public JSONSerializer < Poco::JSON::Array, Poco::JSON::Object > {
protected:

    //----------------------------------------------------------------//
    Mode AbstractSerializer_getMode () override {
        return SERIALIZE_OUT;
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
    
        this->set ( name, ( int )value );
    
    // TODO:
//        char buffer [ 32 ];
//        snprintf ( buffer, 32, "%" PRIx64 "", value );
//        this->set ( name, string ( buffer ).c_str ());
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, string& value ) override {
        this->set ( name, value.c_str ());
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializable& value ) override {
        this->set ( name, toJSON ( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializableArray& value ) override {
    
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array ();
        this->set ( name, array );
    
        ToJSONSerializer serializer;
        serializer.mArray = array;
        value.serialize ( serializer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializablePointer& value ) override {
    
        AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            this->AbstractSerializer_serialize ( name, *serializable );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, Poco::DigestEngine::Digest& value ) override {
        this->set ( name, Poco::DigestEngine::digestToHex ( value ).c_str ());
    }

public:

    //----------------------------------------------------------------//
    static Poco::JSON::Object::Ptr toJSON ( AbstractSerializable& serializable ) {

        Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
        toJSON ( serializable, *object );
        return object;
    }

    //----------------------------------------------------------------//
    static void toJSON ( AbstractSerializable& serializable, Poco::JSON::Object& object ) {

        ToJSONSerializer serializer;
        serializer.mObject = &object;
        serializable.serialize ( serializer );
    }

    //----------------------------------------------------------------//
    static void toJSON ( AbstractSerializable& serializable, ostream& outStream, unsigned int indent = 4, int step = -1 ) {

        Poco::JSON::Object::Ptr object = toJSON ( serializable );
        object->stringify ( outStream, indent, step );
    }
};

} // namespace Volition
#endif
