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
// FeeProfile
//================================================================//
class FeeProfile :
    public AbstractSerializable {
public:

    double      mProfitShare;           // percent of the gratuity offered to miner
    double      mTransferTax;           // percent ot VOL transferred between accounts
    
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
    
        return ( share == ( u64 )ceil (( double )amount * this->mProfitShare ));
    }
    
    //----------------------------------------------------------------//
    bool checkTransferTax ( u64 amount, u64 share ) const {
    
        return ( share == ( u64 )ceil (( double )amount * this->mTransferTax ));
    }

    //----------------------------------------------------------------//
    FeeProfile () :
        mProfitShare ( 0 ),
        mTransferTax ( 0 ) {
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
    
    //----------------------------------------------------------------//
    FeeSchedule () {
    }
};

} // namespace Volition
#endif
