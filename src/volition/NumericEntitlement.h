// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_NUMERICENTITLEMENT_H
#define VOLITION_NUMERICENTITLEMENT_H

#include <volition/common.h>
#include <volition/AbstractEntitlement.h>

namespace Volition {

//================================================================//
// NumericEntitlementLimit
//================================================================//
class NumericEntitlementLimit :
    public AbstractSerializable {
public:

    bool        mEnabled;
    bool        mExclude;   // exclude value of limit
    double      mLimit;

    //----------------------------------------------------------------//
    NumericEntitlementLimit () :
        mEnabled ( false ),
        mExclude ( false ),
        mLimit ( 0 ) {
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "enabled",       this->mEnabled );
        serializer.serialize ( "exclude",       this->mExclude );
        serializer.serialize ( "limit",         this->mLimit );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "enabled",       this->mEnabled );
        serializer.serialize ( "exclude",       this->mExclude );
        serializer.serialize ( "limit",         this->mLimit );
    }
};

//================================================================//
// NumericEntitlement
//================================================================//
class NumericEntitlement :
    public AbstractEntitlement {
protected:

    NumericEntitlementLimit      mLower;
    NumericEntitlementLimit      mUpper;

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check ( double value ) const override {
    
        if ( this->mLower.mEnabled ) {
            double limit = this->mLower.mLimit;
            if (( value < limit ) || ( this->mLower.mExclude && ( value == limit ))) return false;
        }
        
        if ( this->mUpper.mEnabled ) {
            double limit = this->mUpper.mLimit;
            if (( value > limit ) || ( this->mUpper.mExclude && ( value == limit ))) return false;
        }
        return true;
    }

    //----------------------------------------------------------------//
    bool AbstractEntitlement_isMatchOrSubsetOf ( const AbstractEntitlement* abstractOther ) const override {
    
        const NumericEntitlement* other = dynamic_cast < const NumericEntitlement* >( abstractOther );
        if ( !other ) return false;
        
        if ( other->mLower.mEnabled ) {
            if ( !this->mLower.mEnabled ) return false;
            if ( other->mLower.mLimit > this->mLower.mLimit ) return false;
            if ( other->mLower.mLimit == this->mLower.mLimit ) {
                if ( other->mLower.mExclude && ( !this->mLower.mExclude )) return false;
            }
        }
        
        if ( other->mUpper.mEnabled ) {
            if ( !this->mUpper.mEnabled ) return false;
            if ( other->mUpper.mLimit < this->mUpper.mLimit ) return false;
            if ( other->mUpper.mLimit == this->mUpper.mLimit ) {
                if ( other->mUpper.mExclude && ( !this->mUpper.mExclude )) return false;
            }
        }
        
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "lower",     this->mLower );
        serializer.serialize ( "upper",     this->mUpper );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "lower",     this->mLower );
        serializer.serialize ( "upper",     this->mUpper );
    }

public:

    //----------------------------------------------------------------//
    NumericEntitlement () {
    }
    
    //----------------------------------------------------------------//
    ~NumericEntitlement () {
    }
};

} // namespace Volition
#endif
