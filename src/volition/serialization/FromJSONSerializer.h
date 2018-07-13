// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H

#include <volition/serialization/JSONSerializer.h>

namespace Volition {

class AbstractSerializer;

//================================================================//
// FromJSONSerializer
//================================================================//
class FromJSONSerializer :
    public JSONSerializer < const Poco::JSON::Array, const Poco::JSON::Object > {
protected:

    //----------------------------------------------------------------//
    Mode AbstractSerializer_getMode () override {
        return SERIALIZE_IN;
    }

    //----------------------------------------------------------------//
    size_t AbstractSerializer_getSize () override {
        assert ( this->mObject || this->mArray );
        return this->mObject ? this->mObject->size () : this->mArray->size ();
    }

    //----------------------------------------------------------------//
    bool AbstractSerializer_has ( SerializerPropertyName name ) override {
        return this->has ( name );
    }

    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, u64& value ) override {
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
    void AbstractSerializer_serialize ( SerializerPropertyName name, string& value ) override {
        value = this->optValue < string >( name, value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializable& value ) override {
    
        Poco::JSON::Object::Ptr object = this->getObject ( name );
        if ( object ) {
            fromJSON ( value, *object );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializableArray& value ) override {
    
        Poco::JSON::Array::Ptr array = this->getArray ( name );
        if ( array ) {
    
            value.AbstractSerializableArray_resize ( array->size ());
    
            FromJSONSerializer serializer;
            serializer.mArray = array;
            value.serialize ( serializer );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializablePointer& value ) override {
        
        Poco::JSON::Object::Ptr object = this->getObject ( name );
        if ( object ) {
            JSONSerializableTypeInfo typeInfo ( *object );
            value.AbstractSerializablePointer_make ( typeInfo );
            fromJSON ( *value.AbstractSerializablePointer_get (), *object );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, unique_ptr < Poco::Crypto::ECKey >& key ) override {
    
        string keyString = this->optValue < string >( name, "" );
        stringstream keyStream ( keyString );
        key = keyString.size () ? make_unique < Poco::Crypto::ECKey >( &keyStream ) : NULL;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, Poco::DigestEngine::Digest& value ) override {
    
        string digestString = this->optValue < string >( name, "" );
        value = digestString.size () ? Poco::DigestEngine::digestFromHex ( digestString ) : Poco::DigestEngine::Digest ();
    }

public:

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::JSON::Object& object ) {

        FromJSONSerializer serializer;
        serializer.mObject = &object;
        serializable.serialize ( serializer );
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
