// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FIXEDPOINT_H
#define VOLITION_FIXEDPOINT_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// FixedPoint
//================================================================//
class FixedPoint :
    public AbstractSerializable {
public:

    static const u64 DEFAULT_SCALE = 100;

    u64         mFactor;
    u64         mInteger;               // percent of the gratuity offered to miner (in fixed point)
    
    //----------------------------------------------------------------//
    operator bool () const {
    
        return (( this->mFactor > 0 ) && ( this->mInteger > 0 ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "factor",            this->mFactor );
        serializer.serialize ( "integer",           this->mInteger );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "factor",            this->mFactor );
        serializer.serialize ( "integer",           this->mInteger );
    }

    //----------------------------------------------------------------//
    bool checkEquals ( u64 amount, u64 share ) const {
        
        return ( this->computeAndRoundUp ( amount ) == share );
    }
    
    //----------------------------------------------------------------//
    bool checkGreaterEquals ( u64 amount, u64 share ) const {
        
        return ( this->computeAndRoundUp ( amount ) <= share );
    }
    
    //----------------------------------------------------------------//
    u64 computeAndRoundDown ( u64 amount ) const {
    
        if ( this->mFactor == 0 ) return 0;
    
        u128 x = amount * this->mFactor * this->mInteger;
        u128 y = this->mFactor * this->mFactor;
        return ( u64 )( x / y );
    }
    
    //----------------------------------------------------------------//
    u64 computeAndRoundUp ( u64 amount ) const {
        
        if ( this->mFactor == 0 ) return 0;
    
        u128 x = amount * this->mFactor * this->mInteger;
        u128 y = this->mFactor * this->mFactor;
        return ( u64 )(( x / y ) + (( x % y ) ? 1 : 0 ));
    }

    //----------------------------------------------------------------//
    FixedPoint () :
        mFactor ( 0 ),
        mInteger ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    FixedPoint ( u64 scale, u64 percent ) :
        mFactor ( scale ),
        mInteger ( percent ) {
    }
};

} // namespace Volition
#endif
