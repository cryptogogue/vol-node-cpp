// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONFEESCHEDULE_H
#define VOLITION_TRANSACTIONFEESCHEDULE_H

#include <volition/common.h>
#include <volition/FixedPoint.h>
#include <volition/IndexID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// TransactionFeeProfile
//================================================================//
class TransactionFeeProfile :
    public AbstractSerializable {
public:

    FixedPoint  mProfitShare;           // percent of the gratuity offered to miner (in fixed point)
    FixedPoint  mTransferTax;           // percent ot VOL transferred between accounts (in fixed point)
    
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
// TransactionFeeSchedule
//================================================================//
class TransactionFeeSchedule :
    public AbstractSerializable {
public:
        
    TransactionFeeProfile                                   mDefaultProfile;
    SerializableMap < string, TransactionFeeProfile >       mTransactionProfiles;

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
    TransactionFeeProfile getFeeProfile ( string feeType ) {
        
        SerializableMap < string, TransactionFeeProfile >::const_iterator profileIt = this->mTransactionProfiles.find ( feeType );
        if ( profileIt != this->mTransactionProfiles.cend ()) return profileIt->second;
        return this->mDefaultProfile;
    }
};

} // namespace Volition
#endif
