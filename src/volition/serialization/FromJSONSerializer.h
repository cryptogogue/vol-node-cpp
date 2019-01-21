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
    public JSONSerializer < const Poco::JSON::Array, const Poco::JSON::Object, const FromJSONSerializer > {
protected:

    //----------------------------------------------------------------//
    SerializerKeys AbstractSerializerFrom_getKeys () const override {
    
        if ( this->mArray ) {
            return SerializerKeys ( this->mArray->size ());
        }
        
        if ( this->mObject ) {
        
            vector < string > keys;
            keys.reserve ( this->mObject->size ());
        
            Poco::JSON::Object::ConstIterator valueIt = this->mObject->begin ();
            for ( ; valueIt != this->mObject->end (); ++valueIt ) {
                keys.push_back ( valueIt->first );
            }
            return SerializerKeys ( keys );
        }
        
        return SerializerKeys ();
    }
    
    //----------------------------------------------------------------//
    KeyType AbstractSerializerFrom_getKeyType () const override {
    
        if ( this->mArray ) return KEY_TYPE_INDEX;
        if ( this->mObject ) return KEY_TYPE_STRING;
        return KEY_TYPE_NONE;
    }

    //----------------------------------------------------------------//
    SerializerPropertyName AbstractSerializerFrom_getName () const override {
        return this->mName;
    }
    
    //----------------------------------------------------------------//
    const AbstractSerializerFrom* AbstractSerializerFrom_getParent () const override {
        return this->mParent;
    }

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
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, bool& value ) const override {
        value = this->optValue < bool >( name, value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, double& value ) const override {
        value = this->optValue < double >( name, value );
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
    
        if ( this->has ( name )) {
    
            const Poco::Dynamic::Var member = this->get ( name );
            assert ( member );
            
            const type_info& tinfo = member.type ();
            
            if ( tinfo == typeid ( Poco::JSON::Array::Ptr )) {
                fromJSON ( value, *member.extract < Poco::JSON::Array::Ptr >());
            }
            else if ( tinfo == typeid ( Poco::JSON::Object::Ptr )) {
                fromJSON ( value, *member.extract < Poco::JSON::Object::Ptr >());
            }
            else {
                FromJSONSerializer serializer;
                serializer.mParent = this;
                serializer.mName = name;
                value.serializeFrom ( serializer );
            }
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, Variant& value ) const override {
        value = Variant ( this->get ( name ));
    }

public:

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::JSON::Array& array ) {

        FromJSONSerializer serializer;
        serializer.mArray = &array;
        serializable.serializeFrom ( serializer );
    }

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
