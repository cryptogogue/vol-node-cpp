// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SEND_VOL_H
#define VOLITION_TRANSACTIONS_SEND_VOL_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SendVOL
//================================================================//
class SendVOL :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SEND_VOL" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mAccountName; // receiving account
    u64         mAmount;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        Ledger& ledger = context.mLedger;
        const Account& account = context.mAccount;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SEND_VOL )) return false;
        if ( account.mBalance < this->mAmount ) return false;
        
        shared_ptr < Account > recipient = ledger.getAccount ( this->mAccountName );
        if ( !recipient ) return false;
        if ( account.mIndex == recipient->mIndex ) return false;
        
        Account recipientUpdated = *recipient;
        recipientUpdated.mBalance += this->mAmount;
        ledger.setAccount ( recipientUpdated );
        
        if ( !ledger.isGenesis ()) {
            Account accountUpdated = account;
            accountUpdated.mBalance -= this->mAmount;
            ledger.setAccount ( accountUpdated );
        }
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
