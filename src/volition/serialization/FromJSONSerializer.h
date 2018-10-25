// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/JSONSerializer.h>

namespace Volition {

//================================================================//
// FromJSONSerializer
//================================================================//
class FromJSONSerializer :
    public AbstractSerializerFrom,
    public JSONSerializer < const Poco::JSON::Array, const Poco::JSON::Object > {
protected:

    //----------------------------------------------------------------//
    size_t AbstractSerializerFrom_getSize () const override {
        assert ( this->mObject || this->mArray );
        return this->mObject ? this->mObject->size () : this->mArray->size ();
    }

    //----------------------------------------------------------------//
    bool AbstractSerializerFrom_has ( SerializerPropertyName name ) const override {
        return this->has ( name );
    }

    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, u64& value ) const override {
        value = this->optValue < u64 >( name, value );

        // TODO:
//        string strValue = object.optValue < string >( key, "" );
//        if ( strValue.size () > 0 ) {
//
//            assert ( sizeof ( unsigned long long int ) == sizeof ( u64 ));
//
//            errno = 0;
//            u64 value = strtoull ( strValue.c_str (), NULL, 16 );
//            if ( errno == 0 ) {
//                result = value;
//            }
//        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, string& value ) const override {
        value = this->optValue < string >( name, value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, AbstractSerializable& value ) const override {
    
        Poco::JSON::Object::Ptr object = this->getObject ( name );
        if ( object ) {
            fromJSON ( value, *object );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, AbstractSerializableArray& value ) const override {
    
        Poco::JSON::Array::Ptr array = this->getArray ( name );
        if ( array ) {
    
            value.AbstractSerializableArray_resize ( array->size ());
    
            FromJSONSerializer serializer;
            serializer.mArray = array;
            value.serializeFrom ( serializer );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, AbstractSerializablePointer& value ) const override {
        
        Poco::JSON::Object::Ptr object = this->getObject ( name );
        if ( object ) {
            JSONSerializableTypeInfo typeInfo ( *object );
            value.AbstractSerializablePointer_make ( typeInfo );
            fromJSON ( *value.AbstractSerializablePointer_get (), *object );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, AbstractStringifiable& value ) const override {
    
        value.fromString ( this->optValue < string >( name, "" ));
    }

public:

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::JSON::Object& object ) {

        FromJSONSerializer serializer;
        serializer.mObject = &object;
        serializable.serializeFrom ( serializer );
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, istream& inStream ) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( inStream );
        Poco::JSON::Object::Ptr object = result.extract < Poco::JSON::Object::Ptr >();

        fromJSON ( serializable, *object );
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, string json ) {

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( json );
        Poco::JSON::Object::Ptr object = result.extract < Poco::JSON::Object::Ptr >();

        fromJSON ( serializable, *object );
    }
};

} // namespace Volition
#endif
