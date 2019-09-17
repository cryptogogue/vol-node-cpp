// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BOOLEANENTITLEMENT_H
#define VOLITION_BOOLEANENTITLEMENT_H

#include <volition/common.h>
#include <volition/AbstractEntitlement.h>

namespace Volition {

//================================================================//
// BooleanEntitlement
//================================================================//
class BooleanEntitlement :
    public AbstractEntitlement {
protected:

    bool    mValue;

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check () const override {
    
        return this->mValue;
    }
    
    //----------------------------------------------------------------//
    bool AbstractEntitlement_check ( double value ) const override {
        UNUSED ( value );
    
        return this->mValue;
    }

    //----------------------------------------------------------------//
    shared_ptr < AbstractEntitlement > AbstractEntitlement_clone () const override {
    
        return make_shared < BooleanEntitlement >( *this );
    }

    //----------------------------------------------------------------//
    Leniency AbstractEntitlement_compare ( const AbstractEntitlement* abstractOther ) const override {
        
        return AbstractEntitlement::compareRanges ( this, abstractOther );
    }
    
    //----------------------------------------------------------------//
    Range AbstractEntitlement_getRange () const override {
    
        return this->mValue ? ALWAYS_TRUE : ALWAYS_FALSE;
    }

    //----------------------------------------------------------------//
    void AbstractEntitlement_print () const override {
        printf ( "%s", this->mValue ? "TRUE" : "FALSE" );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "value",     this->mValue );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",      ENTITLEMENT_TYPE_BOOLEAN );
        serializer.serialize ( "value",     this->mValue );
    }

public:

    //----------------------------------------------------------------//
    BooleanEntitlement () :
        mValue ( false ) {
    }
    
    //----------------------------------------------------------------//
    BooleanEntitlement ( bool value ) :
        mValue ( value ) {
    }
    
    //----------------------------------------------------------------//
    ~BooleanEntitlement () {
    }
};

} // namespace Volition
#endif
