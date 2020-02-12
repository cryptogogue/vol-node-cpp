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
    bool check ( double value, bool isLowerLimit ) const {
    
        if ( this->mEnabled ) {
            if ( this->mExclude && ( value == this->mLimit )) return false;
            return isLowerLimit ? ( value > this->mLimit ) : ( value < this->mLimit );
        }
        return true;
    }

    //----------------------------------------------------------------//
    int compare ( const NumericEntitlementLimit& other, bool isLowerLimit ) const {
        
        if ( other.mEnabled ) {
            if ( !this->mEnabled ) return AbstractEntitlement::IS_LESS_RESTRICTIVE;
            
            if ( this->mLimit == other.mLimit ) {
                if ( this->mExclude == other.mExclude ) return AbstractEntitlement::IS_EQUALLY_RESTRICTIVE;
                if ( this->mExclude && !other.mExclude ) return AbstractEntitlement::IS_MORE_RESTRICTIVE;
                return AbstractEntitlement::IS_LESS_RESTRICTIVE;
            }
            
            if ( isLowerLimit ) {
                return ( this->mLimit < other.mLimit ) ? AbstractEntitlement::IS_LESS_RESTRICTIVE : AbstractEntitlement::IS_MORE_RESTRICTIVE;
            }
            return ( this->mLimit > other.mLimit ) ? AbstractEntitlement::IS_LESS_RESTRICTIVE : AbstractEntitlement::IS_MORE_RESTRICTIVE;
        }
        return this->mEnabled ? AbstractEntitlement::IS_MORE_RESTRICTIVE : AbstractEntitlement::IS_EQUALLY_RESTRICTIVE;
    }

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

    enum LimitType {
        NUMERIC_ANY,
        NUMERIC_LOWER_LIMIT,
        NUMERIC_UPPER_LIMIT,
        NUMERIC_RANGE,
    };

    NumericEntitlementLimit      mLower;
    NumericEntitlementLimit      mUpper;

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check () const override {
    
        return !( this->mLower.mEnabled || this->mUpper.mEnabled );
    }

    //----------------------------------------------------------------//
    bool AbstractEntitlement_check ( double value ) const override {
    
        return ( this->mLower.check ( value, true ) && this->mUpper.check ( value, false ));
    }
    
    //----------------------------------------------------------------//
    shared_ptr < AbstractEntitlement > AbstractEntitlement_clone () const override {
    
        return make_shared < NumericEntitlement >( *this );
    }

    //----------------------------------------------------------------//
    Leniency AbstractEntitlement_compare ( const AbstractEntitlement* abstractOther ) const override {
    
        const NumericEntitlement* otherNumeric = dynamic_cast < const NumericEntitlement* >( abstractOther );
        if ( otherNumeric ) {
        
            int lowerCompare = this->mLower.compare ( otherNumeric->mLower, true );
            int upperCompare = this->mUpper.compare ( otherNumeric->mUpper, false );
            
            // if *either* constraint is less restrictive, then we are less restrictive
            if (( lowerCompare == IS_LESS_RESTRICTIVE ) || ( upperCompare == IS_LESS_RESTRICTIVE )) return IS_LESS_RESTRICTIVE;
            
            // if *both* constraints are equally restrictive, then we are equally restrictive
            if (( lowerCompare == IS_EQUALLY_RESTRICTIVE ) && ( upperCompare == IS_EQUALLY_RESTRICTIVE )) return IS_EQUALLY_RESTRICTIVE;
            
            return IS_MORE_RESTRICTIVE;
        }
        return AbstractEntitlement::compareRanges ( this, abstractOther );
    }

    //----------------------------------------------------------------//
    Range AbstractEntitlement_getRange () const override {
    
        return ( this->mLower.mEnabled || this->mUpper.mEnabled ) ? MAY_BE_TRUE_OR_FALSE : ALWAYS_TRUE;
    }

    //----------------------------------------------------------------//
    void AbstractEntitlement_print () const override {
        
        switch ( this->getLimitType ()) {
            case NUMERIC_ANY:
                printf ( "*" );
                break;
            case NUMERIC_LOWER_LIMIT:
                printf ( "%s %f", this->mLower.mExclude ? ">" : ">=", this->mLower.mLimit );
                break;
            case NUMERIC_UPPER_LIMIT:
                printf ( "%s %f", this->mUpper.mExclude ? "<" : "<=", this->mUpper.mLimit );
                break;
            case NUMERIC_RANGE:
                printf ( "%s%f, %f%s",
                    this->mLower.mExclude ? "(" : "[",
                    this->mLower.mLimit,
                    this->mUpper.mLimit,
                    this->mUpper.mExclude ? ")" : "]"
                );
                break;
        }
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "lower",     this->mLower );
        serializer.serialize ( "upper",     this->mUpper );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "type",      ENTITLEMENT_TYPE_NUMERIC );
        serializer.serialize ( "lower",     this->mLower );
        serializer.serialize ( "upper",     this->mUpper );
    }

    //----------------------------------------------------------------//
    LimitType getLimitType () const {
    
        if ( this->mLower.mEnabled && this->mUpper.mEnabled ) return NUMERIC_RANGE;
        if ( this->mLower.mEnabled ) return NUMERIC_LOWER_LIMIT;
        if ( this->mUpper.mEnabled ) return NUMERIC_UPPER_LIMIT;
        return NUMERIC_ANY;
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
