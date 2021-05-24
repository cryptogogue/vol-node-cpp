// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_PAYOUTPOLICY_H
#define VOLITION_PAYOUTPOLICY_H

#include <volition/common.h>
#include <volition/AccountODBM.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// PayoutPolicy
//================================================================//
class PayoutPolicy :
    public AbstractSerializable {
private:

    static const u64 DEFAULT_SCALE          = 100;

    u64                                     mScale;
    SerializableMap < string, u64 >         mShares;

public:

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "scale",         this->mScale );
        serializer.serialize ( "shares",        this->mShares );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "scale",         this->mScale );
        serializer.serialize ( "shares",        this->mShares );
    }

    //----------------------------------------------------------------//
    static u64 countDecimals ( double v ) {
    
        u64 decimals = 1;
        while (( v - floor ( v )) != 0 ) {
            v *= 10;
            decimals *= 10;
        }
        return decimals;
    }
    
    //----------------------------------------------------------------//
    bool hasAccounts ( const AbstractLedger& ledger ) const {
    
        SerializableMap < string, u64 >::const_iterator shareIt = this->mShares.cbegin ();
        for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            string accountName  = shareIt->first;
            AccountODBM accountODBM ( ledger, accountName );
            if ( !accountODBM ) return false;
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    bool isBalanced () const {
    
        double balance = 0;
        SerializableMap < string, u64 >::const_iterator shareIt = this->mShares.cbegin ();
        for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            balance += shareIt->second;
        }
        return ( balance == this->mScale );
    }
    
    //----------------------------------------------------------------//
    void payout ( AbstractLedger& ledger ) const {
        
        u64 pool = ledger.getPayoutPool ();
        if ( pool == 0 ) return;
        
        if ( !this->isBalanced ()) return;
        if ( !this->hasAccounts ( ledger )) return;
        
        u64 dist = ( u64 )( pool / this->mScale );

        if ( dist > 0 ) {
            
            u64 totalDistributed = 0;

            SerializableMap < string, u64 >::const_iterator shareIt = this->mShares.cbegin ();
            for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            
                string accountName  = shareIt->first;
                u64 distribution    = ( u64 )( shareIt->second * dist );
                
                AccountODBM accountODBM ( ledger, accountName );
                assert ( accountODBM );
                
                accountODBM.mBalance.set ( accountODBM.mBalance.get () + distribution );
                totalDistributed += distribution;
            }
            ledger.setPayoutPool ( pool - totalDistributed );
        }
    }

    //----------------------------------------------------------------//
    PayoutPolicy () :
        mScale ( DEFAULT_SCALE ) {
    }
    
    //----------------------------------------------------------------//
    void setScale ( u64 scale ) {
        this->mScale = scale;
    }
    
    //----------------------------------------------------------------//
    void setShare ( string name, u64 share ) {
        this->mShares [ name ] = share;
    }
};

} // namespace Volition
#endif
