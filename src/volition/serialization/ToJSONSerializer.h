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
    bool AbstractSerializerTo_isDigest () const override {
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
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractStringifiable& value ) override {
        this->set ( name, value.toString ().c_str ());
    }

public:

    //----------------------------------------------------------------//
    static Poco::Dynamic::Var toJSON ( const AbstractSerializable& serializable ) {

        ToJSONSerializer serializer;
        serializable.serializeTo ( serializer );
        return serializer;
    }

    //----------------------------------------------------------------//
    static void toJSON ( const AbstractSerializable& serializable, ostream& outStream, unsigned int indent = 4, int step = -1 ) {

        Poco::Dynamic::Var json = toJSON ( serializable );
        
        Poco::JSON::Object::Ptr object = json.extract < Poco::JSON::Object::Ptr >();
        if ( object ) {
            object->stringify ( outStream, indent, step );
        }
        else {
            Poco::JSON::Array::Ptr array = json.extract < Poco::JSON::Array::Ptr >();
            if ( array ) {
                array->stringify ( outStream, indent, step );
            }
        }
    }
};

} // namespace Volition
#endif
