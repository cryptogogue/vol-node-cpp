// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_FROMJSONSERIALIZER_H

#include <volition/serialization/AbstractSerializerFrom.h>

namespace Volition {

//================================================================//
// FromJSONSerializer
//================================================================//
class FromJSONSerializer :
    public AbstractSerializerFrom {
protected:

    friend class SerializableOpaque;

    const Poco::JSON::Array*    mArray;
    const Poco::JSON::Object*   mObject;
    const FromJSONSerializer*   mParent;
    
    SerializerPropertyName      mName;

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

        Poco::Dynamic::Var var = this->get ( name );
        
        if ( !var.isEmpty ()) {
            if ( var.type () == typeid ( string )) {
            
                string strValue = var.convert < string >();
                if ( strValue.size () > 2 ) {
                    errno = 0;
                    u64 result = strtoull ( &strValue.c_str ()[ 2 ], NULL, 16 );
                    if ( errno == 0 ) {
                        value = result;
                    }
                }
            }
            else {
                value = var.convert < u64 >();
            }
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, string& value ) const override {
        value = this->optValue < string >( name, value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, AbstractSerializable& value ) const override {
    
        if ( this->has ( name )) {
    
            FromJSONSerializer serializer;
            serializer.mParent = this;
            serializer.mName = name;
    
            const Poco::Dynamic::Var member = this->get ( name );
            assert ( !member.isEmpty ());
           
            const type_info& tinfo = member.type ();
           
            if ( tinfo == typeid ( Poco::JSON::Array::Ptr )) {
                Poco::JSON::Array::Ptr array = member.extract < Poco::JSON::Array::Ptr >();
                assert ( array );
                serializer.mArray = array;
            }
            else if ( tinfo == typeid ( Poco::JSON::Object::Ptr )) {
                Poco::JSON::Object::Ptr object = member.extract < Poco::JSON::Object::Ptr >();
                assert ( object );
                serializer.mObject = object;
            }
            value.serializeFrom ( serializer );
        }
        else {
        
            value.serializeFrom ();
        }
    }

    //----------------------------------------------------------------//
    void AbstractSerializerFrom_serialize ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) const override {
    
        if ( this->has ( name )) {
    
            FromJSONSerializer serializer;
            serializer.mParent = this;
            serializer.mName = name;
    
            const Poco::Dynamic::Var member = this->get ( name );
            assert ( !member.isEmpty ());
           
            const type_info& tinfo = member.type ();
           
            if ( tinfo == typeid ( Poco::JSON::Array::Ptr )) {
                Poco::JSON::Array::Ptr array = member.extract < Poco::JSON::Array::Ptr >();
                assert ( array );
                serializer.mArray = array;
            }
            else if ( tinfo == typeid ( Poco::JSON::Object::Ptr )) {
                Poco::JSON::Object::Ptr object = member.extract < Poco::JSON::Object::Ptr >();
                assert ( object );
                serializer.mObject = object;
            }
            serializeFunc ( serializer );
        }
    };

    //----------------------------------------------------------------//
    void AbstractSerializerFrom_stringFromTree ( SerializerPropertyName name, string& value ) const override {
    
        if ( this->has ( name )) {
    
            const Poco::Dynamic::Var member = this->get ( name );
            assert ( !member.isEmpty ());
           
            const type_info& tinfo = member.type ();
           
            stringstream strStream;
           
            if ( tinfo == typeid ( Poco::JSON::Array::Ptr )) {
                Poco::JSON::Array::Ptr array = member.extract < Poco::JSON::Array::Ptr >();
                array->stringify ( strStream );
            }
            else if ( tinfo == typeid ( Poco::JSON::Object::Ptr )) {
                Poco::JSON::Object::Ptr object = member.extract < Poco::JSON::Object::Ptr >();
                object->stringify ( strStream );
            }
            value = strStream.str ();
        }
    }

    //----------------------------------------------------------------//
    const Poco::Dynamic::Var get ( SerializerPropertyName name ) const {
    
        if ( this->mArray ) {
            return this->mArray->get (( unsigned int )name.getIndex ());
        }
        assert ( this->mObject );
        return this->mObject->get ( name.getName ());
    }

    //----------------------------------------------------------------//
    bool has ( SerializerPropertyName name ) const {
    
        if ( this->mArray ) {
            return name.getIndex () < this->mArray->size ();
        }
        assert ( this->mObject );
        return this->mObject->has ( name.getName ());
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    TYPE optValue ( SerializerPropertyName name, const TYPE& fallback ) const {
    
        assert ( this->mObject || this->mArray );
    
        Poco::Dynamic::Var value = this->get ( name );
        
        if ( !value.isEmpty ()) {
            try {
                return value.convert < TYPE >();
            }
            catch ( ... ) {
            }
        }
        return fallback;
    }

public:

    //----------------------------------------------------------------//
    FromJSONSerializer () :
        mArray ( NULL ),
        mObject ( NULL ),
        mParent ( NULL ) {
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::JSON::Array& array ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        FromJSONSerializer serializer;
        serializer.mArray = &array;
        serializable.serializeFrom ( serializer );
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::JSON::Object& object ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        FromJSONSerializer serializer;
        serializer.mObject = &object;
        serializable.serializeFrom ( serializer );
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, const Poco::Dynamic::Var& var ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        if ( var.type () == typeid ( Poco::JSON::Object::Ptr )) {
            Poco::JSON::Object::Ptr object = var.extract < Poco::JSON::Object::Ptr >();
            fromJSON ( serializable, *object );
        }
        else if ( var.type () == typeid ( Poco::JSON::Array::Ptr )) {
            Poco::JSON::Array::Ptr array = var.extract < Poco::JSON::Array::Ptr >();
            fromJSON ( serializable, *array );
        }
    }

    //----------------------------------------------------------------//
    static void fromJSON ( AbstractSerializable& serializable, istream& inStream ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( inStream );
        
        FromJSONSerializer::fromJSON ( serializable, result );
    }

    //----------------------------------------------------------------//
    static void fromJSONFile ( AbstractSerializable& serializable, string filename ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        fstream inStream;
        inStream.open ( filename, ios_base::in );
        FromJSONSerializer::fromJSON ( serializable, inStream );
        inStream.close ();
    }

    //----------------------------------------------------------------//
    static void fromJSONString ( AbstractSerializable& serializable, string json ) {

        LGN_LOG_SCOPE ( VOL_FILTER_JSON, INFO, __PRETTY_FUNCTION__ );

        istringstream str ( json );
        FromJSONSerializer::fromJSON ( serializable, str );
    }
};

} // namespace Volition
#endif
