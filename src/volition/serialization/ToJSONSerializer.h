// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_TOJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_TOJSONSERIALIZER_H

#include <volition/serialization/AbstractSerializerTo.h>
#include <volition/serialization/JSONSerializer.h>

namespace Volition {

//================================================================//
// ToJSONSerializer
//================================================================//
class ToJSONSerializer :
    public AbstractSerializerTo,
    public JSONSerializer < Poco::JSON::Array, Poco::JSON::Object > {
protected:

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () override {
        return false;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const u64& value ) override {
    
        this->set ( name, ( int )value );
    
    // TODO:
//        char buffer [ 32 ];
//        snprintf ( buffer, 32, "%" PRIx64 "", value );
//        this->set ( name, string ( buffer ).c_str ());
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const string& value ) override {
        this->set ( name, value.c_str ());
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializable& value ) override {
        this->set ( name, toJSON ( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializableArray& value ) override {
    
        Poco::JSON::Array::Ptr array = new Poco::JSON::Array ();
        this->set ( name, array );
    
        ToJSONSerializer serializer;
        serializer.mArray = array;
        value.serializeTo ( serializer );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializablePointer& value ) override {
    
        const AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            this->AbstractSerializerTo_serialize ( name, *serializable );
        }
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractStringifiable& value ) override {
    
        this->set ( name, value.toString ().c_str ());
    }

public:

    //----------------------------------------------------------------//
    static Poco::JSON::Object::Ptr toJSON ( const AbstractSerializable& serializable ) {

        Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
        toJSON ( serializable, *object );
        return object;
    }

    //----------------------------------------------------------------//
    static void toJSON ( const AbstractSerializable& serializable, Poco::JSON::Object& object ) {

        ToJSONSerializer serializer;
        serializer.mObject = &object;
        serializable.serializeTo ( serializer );
    }

    //----------------------------------------------------------------//
    static void toJSON ( const AbstractSerializable& serializable, ostream& outStream, unsigned int indent = 4, int step = -1 ) {

        Poco::JSON::Object::Ptr object = toJSON ( serializable );
        object->stringify ( outStream, indent, step );
    }
};

} // namespace Volition
#endif
