// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H
#define VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/ContractWithDigest.h>
#include <volition/Gamercert.h>
#include <volition/IdentityProvider.h>
#include <volition/Policy.h>
#include <volition/Signature.h>

namespace Volition {
namespace Transactions {

//================================================================//
// IdentifyAccount
//================================================================//
class IdentifyAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "IDENTIFY_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    Gamercert       mGamercert;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "identity",          this->mGamercert );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "identity",          this->mGamercert );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::IDENTIFY_ACCOUNT )) return "Permission denied.";

        AbstractLedger& ledger = context.mLedger;
        
        string identityProviderName = this->mGamercert.mKeyName;
        IdentityProvider identityProvider = ledger.getIdentityProvider ( identityProviderName );
        if ( !identityProvider ) return "Missing identity policy for identity provider certificate signing key.";
        
        string fingerprint;
        
        TransactionResult identityResult = this->mGamercert.verify (
            identityProvider.mEd25519PublicHex,
            ledger.getIdentity (),
            CryptoKeyInfo ( context.mKeyAndPolicy.mKey.getKeyPair ()).mPublicKey,
            fingerprint
        );
        if ( !identityResult ) return identityResult;

        context.mAccountODBM.setIdentity ( fingerprint, identityProviderName );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
