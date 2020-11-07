// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FEEDISTRIBUTIONTABLE_H
#define VOLITION_FEEDISTRIBUTIONTABLE_H

#include <volition/common.h>
#include <volition/AccountODBM.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// FeeDistributionTable
//================================================================//
class FeeDistributionTable :
    public AbstractSerializable {
private:

    SerializableMap < string, double >      mShares;

public:

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "shares",        this->mShares );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "shares",        this->mShares );
    }

    //----------------------------------------------------------------//
    u64 countDecimals ( double v ) const {
    
        u64 decimals = 1;
        while (( v - floor ( v )) != 0 ) {
            v *= 10;
            decimals *= 10;
        }
        return decimals;
    }

    //----------------------------------------------------------------//
    void distribute ( Ledger& ledger ) const {
        
        u64 pool = ledger.getFeeDistributionPool ();
        if ( pool == 0 ) return;
        
        if ( !this->isBalanced ()) return;
        if ( !this->hasAccounts ( ledger )) return;
        
        u64 decimals = this->findDecimals ();
        u64 dist = ( u64 )floor ( pool / decimals );

        if ( dist > 0 ) {
            
            u64 totalDistributed = 0;

            SerializableMap < string, double >::const_iterator shareIt = this->mShares.cbegin ();
            for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            
                string accountName  = shareIt->first;
                u64 distribution    = ( u64 )( shareIt->second * decimals * dist );
                
                AccountODBM accountODBM ( ledger, accountName );
                assert ( accountODBM );
                
                Account account = *accountODBM.mBody.get ();
                account.mBalance += distribution;
                accountODBM.mBody.set ( account );

                totalDistributed += distribution;
            }
            ledger.setFeeDistributionPool ( pool - totalDistributed );
        }
    }

    //----------------------------------------------------------------//
    u64 findDecimals () const {
        
        if ( !this->mShares.size ()) return 0;
        
        u64 max = 0;
        SerializableMap < string, double >::const_iterator shareIt = this->mShares.cbegin ();
        for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            u64 decimals = countDecimals ( shareIt->second );
            max = max < decimals ? decimals : max;
        }
        return max;
    }
    
    //----------------------------------------------------------------//
    bool hasAccounts ( const Ledger& ledger ) const {
    
        SerializableMap < string, double >::const_iterator shareIt = this->mShares.cbegin ();
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
        SerializableMap < string, double >::const_iterator shareIt = this->mShares.cbegin ();
        for ( ; shareIt != this->mShares.cend (); ++shareIt ) {
            balance += shareIt->second;
        }
        return ( balance == 1 );
    }
    
    //----------------------------------------------------------------//
    void setShare ( string name, double share ) {
        this->mShares [ name ] = share;
    }
};

} // namespace Volition
#endif
