// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSTRINGIFIABLE_H
#define VOLITION_SERIALIZATION_ABSTRACTSTRINGIFIABLE_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// AbstractStringifiable
//================================================================//
class AbstractStringifiable :
    virtual public AbstractSerializable {
protected:

    //----------------------------------------------------------------//
    virtual void        AbstractStringifiable_fromString    ( string str ) = 0;
    virtual string      AbstractStringifiable_toString      () const = 0;

public:

     //----------------------------------------------------------------//
    AbstractStringifiable& operator = ( string str ) {
        this->fromString ( str );
        return *this;
    }
    
    //----------------------------------------------------------------//
    AbstractStringifiable& operator = ( const char* str ) {
        this->fromString ( string ( str ));
        return *this;
    }
    
    //----------------------------------------------------------------//
    operator string () const {
        return this->toString ();
    }

    //----------------------------------------------------------------//
    AbstractStringifiable () {
    }
    
    //----------------------------------------------------------------//
    AbstractStringifiable ( string str ) {
        this->fromString ( str );
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractStringifiable () {
    }

    //----------------------------------------------------------------//
    void fromString ( string str ) {
        this->AbstractStringifiable_fromString ( str );
    }

    //----------------------------------------------------------------//
    string toString () const {
        return this->AbstractStringifiable_toString ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        const AbstractSerializerFrom* parent = serializer.getParent ();
        if ( parent ) {
            string value;
            parent->serialize ( serializer.getName (), value );
            this->fromString ( value );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        AbstractSerializerTo* parent = serializer.getParent ();
        if ( parent ) {
            parent->serialize ( serializer.getName (), this->toString ());
        }
    }
};

} // namespace Volition
#endif
