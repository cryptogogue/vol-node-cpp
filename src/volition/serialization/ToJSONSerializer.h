// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_TOJSONSERIALIZER_H
#define VOLITION_SERIALIZATION_TOJSONSERIALIZER_H

#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// ToJSONSerializer
//================================================================//
class ToJSONSerializer :
    public AbstractSerializerTo {
protected:

    Poco::JSON::Array::Ptr      mArray;
    Poco::JSON::Object::Ptr     mObject;
    ToJSONSerializer*           mParent;
    
    SerializerPropertyName      mName;

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mArray || this->mObject );
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_affirmArray () override {
        this->affirmJSONArray ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_affirmObject () override {
        this->affirmJSONObject ();
    }

    //----------------------------------------------------------------//
    SerializerPropertyName AbstractSerializerTo_getName () const override {
        return this->mName;
    }
    
    //----------------------------------------------------------------//
    AbstractSerializerTo* AbstractSerializerTo_getParent () override {
        return this->mParent;
    }

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () const override {
        return false;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const bool& value ) override {
        this->set ( name, value );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const double& value ) override {
        this->set ( name, value );
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
    
        ToJSONSerializer serializer;
        serializer.mParent = this;
        serializer.mName = name;
        value.serialize ( serializer );
        
        if ( serializer ) {
            Poco::Dynamic::Var var = serializer.mArray ? ( Poco::Dynamic::Var )serializer.mArray : ( Poco::Dynamic::Var )serializer.mObject;
            this->set ( name, var );
        }
    }

    //----------------------------------------------------------------//
    void affirmJSONArray () {
        assert ( !this->mObject );
        if ( !this->mArray ) {
            this->mArray = new Poco::JSON::Array ();
        }
    }
    
    //----------------------------------------------------------------//
    void affirmJSONObject () {
        assert ( !this->mArray );
        if ( !this->mObject ) {
            this->mObject = new Poco::JSON::Object ();
        }
    }

    //----------------------------------------------------------------//
    void set ( SerializerPropertyName name, const Poco::Dynamic::Var& value ) {
        
        if ( name.isIndex ()) {
            this->affirmJSONArray ();
            this->mArray->set (( unsigned int )name.getIndex (), value );
        }
        else {
            this->affirmJSONObject ();
            this->mObject->set ( name.getName (), value );
        }
    }

public:

    //----------------------------------------------------------------//
    ToJSONSerializer () :
        mParent ( NULL ) {
    }

    //----------------------------------------------------------------//
    static Poco::Dynamic::Var toJSON ( const AbstractSerializable& serializable ) {

        ToJSONSerializer serializer;
        serializable.serialize ( serializer );
        
        if ( serializer ) {
            if ( serializer.mArray ) {
                return Poco::JSON::Array::Ptr ( serializer.mArray );
            }
            return Poco::JSON::Object::Ptr ( serializer.mObject );
        }
        return Poco::Dynamic::Var ();
    }

    //----------------------------------------------------------------//
    static void toJSON ( const AbstractSerializable& serializable, ostream& outStream, unsigned int indent = 4, int step = -1 ) {

        Poco::Dynamic::Var json = toJSON ( serializable );
        
        if ( json.type () == typeid ( Poco::JSON::Object::Ptr )) {
            Poco::JSON::Object::Ptr object = json.extract < Poco::JSON::Object::Ptr >();
            object->stringify ( outStream, indent, step );
        }
        else if ( json.type () == typeid ( Poco::JSON::Array::Ptr )) {
            Poco::JSON::Array::Ptr array = json.extract < Poco::JSON::Array::Ptr >();
            array->stringify ( outStream, indent, step );
        }
    }
    
    //----------------------------------------------------------------//
    static string toJSONString ( const AbstractSerializable& serializable ) {

        stringstream strStream;
        ToJSONSerializer::toJSON ( serializable, strStream, 0 );
        return strStream.str ();
    }
};

} // namespace Volition
#endif
