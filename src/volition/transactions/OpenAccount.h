// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_OPENACCOUNT_H
#define VOLITION_TRANSACTIONS_OPENACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Munge.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string                              mSuffix;        // child name formatted <hex3>.<hex3>.<hex3>
    CryptoPublicKey                     mKey;           // key
    u64                                 mGrant;         // amount to fund
    SerializableSharedPtr < Policy >    mAccountPolicy;
    SerializableSharedPtr < Policy >    mKeyPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "suffix",            this->mSuffix );
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "grant",             this->mGrant );
        serializer.serialize ( "accountPolicy",     this->mAccountPolicy );
        serializer.serialize ( "keyPolicy",         this->mKeyPolicy );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "suffix",            this->mSuffix );
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "grant",             this->mGrant );
        serializer.serialize ( "accountPolicy",     this->mAccountPolicy );
        serializer.serialize ( "keyPolicy",         this->mKeyPolicy );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        AbstractLedger& ledger = context.mLedger;
        const Account& account = context.mAccount;
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::OPEN_ACCOUNT )) return "Permission denied.";
        
        string sponsorName = context.mAccountODBM.mName.get ();
        string suffix = this->mSuffix;
        const KeyAndPolicy& sponsorKeyAndPolicy = context.mKeyAndPolicy;
        
        if ( Ledger::isChildName ( sponsorName )) return "Cannot sponsor from child account.";
        if ( !Ledger::isSuffix ( suffix )) return "Account name suffix error.";
        
        // the child name will be prepended with the sponsor name following a dot: ".<sponsorName>.<childSuffix>"
        // i.e. ".maker.000.000.000"
        // this prevents it from sponsoring any new accounts until it is renamed.

        string childName = Format::write ( ".%s.%s", sponsorName.c_str (), suffix.c_str ());
        assert ( Ledger::isChildName ( childName ));

        const Policy* keyBequest = ledger.resolveBequest < KeyEntitlements >( sponsorKeyAndPolicy.mPolicy, sponsorKeyAndPolicy.getBequest (), this->mKeyPolicy.get ());
        if ( !keyBequest ) return "Missing account key bequest.";
        
        const Policy* accountBequest = ledger.resolveBequest < AccountEntitlements >( account.mPolicy, account.getBequest (), this->mAccountPolicy.get ());
        if ( !accountBequest ) return "Missing account bequest.";

        if ( !ledger.newAccount ( childName, this->mGrant, Ledger::MASTER_KEY_NAME, this->mKey, *keyBequest, *accountBequest )) return "Failed to create account.";;

        context.mAccountODBM.subFunds ( this->mGrant );

        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_sendVOL () const override {
    
        return this->mGrant;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
