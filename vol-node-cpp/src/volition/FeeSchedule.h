// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FEESCHEDULE_H
#define VOLITION_FEESCHEDULE_H

#include <volition/common.h>
#include <volition/AccountID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// FeePercent
//================================================================//
class FeePercent :
    public AbstractSerializable {
public:

    static const u64 DEFAULT_SCALE = 100;

    u64         mScale;
    u64         mPercent;               // percent of the gratuity offered to miner (in fixed point)
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "scale",             this->mScale );
        serializer.serialize ( "percent",           this->mPercent );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "scale",             this->mScale );
        serializer.serialize ( "percent",           this->mPercent );
    }

    //----------------------------------------------------------------//
    bool check ( u64 amount, u64 share ) const {
    
        u64 amountF     = amount * this->mScale;
        u64 checkF      = ( u64 )floor (( amountF * this->mPercent ) / this->mScale );
        u64 check       = ( u64 )floor ( checkF / this->mScale ) + ((( checkF % this->mScale ) == 0 ) ? 0 : 1 );
        
        return ( share == check );
    }
    
    //----------------------------------------------------------------//
    FeePercent () :
        mScale ( DEFAULT_SCALE ),
        mPercent ( 0 ) {
    }
};

//================================================================//
// FeeProfile
//================================================================//
class FeeProfile :
    public AbstractSerializable {
public:

    FeePercent  mProfitShare;           // percent of the gratuity offered to miner (in fixed point)
    FeePercent  mTransferTax;           // percent ot VOL transferred between accounts (in fixed point)
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "profitShare",       this->mProfitShare );
        serializer.serialize ( "transferTax",       this->mTransferTax );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "profitShare",       this->mProfitShare );
        serializer.serialize ( "transferTax",       this->mTransferTax );
    }

    //----------------------------------------------------------------//
    bool checkProfitShare ( u64 amount, u64 share ) const {
    
        return this->mProfitShare.check ( amount, share );
    }
    
    //----------------------------------------------------------------//
    bool checkTransferTax ( u64 amount, u64 share ) const {
    
        return this->mTransferTax.check ( amount, share );
    }
};

//================================================================//
// FeeSchedule
//================================================================//
class FeeSchedule :
    public AbstractSerializable {
public:
    
    FeeProfile                                  mDefaultProfile;
    SerializableMap < string, FeeProfile >      mTransactionProfiles;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "defaultProfile",        this->mDefaultProfile );
        serializer.serialize ( "transactionProfiles",   this->mTransactionProfiles );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "defaultProfile",        this->mDefaultProfile );
        serializer.serialize ( "transactionProfiles",   this->mTransactionProfiles );
    }
    
    //----------------------------------------------------------------//
    FeeProfile getFeeProfile ( string feeType ) {
        
        SerializableMap < string, FeeProfile >::const_iterator profileIt = this->mTransactionProfiles.find ( feeType );
        if ( profileIt != this->mTransactionProfiles.cend ()) return profileIt->second;
        return this->mDefaultProfile;
    }
};

} // namespace Volition
#endif
