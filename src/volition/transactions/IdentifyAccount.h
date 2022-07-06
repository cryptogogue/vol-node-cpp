// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H
#define VOLITION_TRANSACTIONS_IDENTIFYACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
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
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "IDENTIFY_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )
    TRANSACTION_MIN_RELEASE ( 4 )

    string              mProvider;
    u64                 mGrant;
    Gamercert           mGamercert;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "provider",          this->mProvider );
        serializer.serialize ( "grant",             this->mGrant );
        serializer.serialize ( "identity",          this->mGamercert );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "provider",          this->mProvider );
        serializer.serialize ( "grant",             this->mGrant );
        serializer.serialize ( "identity",          this->mGamercert );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::IDENTIFY_ACCOUNT )) return "Permission denied.";

        AbstractLedger& ledger = context.mLedger;
        
        IdentityProvider identityProvider = ledger.getIdentityProvider ( this->mProvider );
        if ( !identityProvider ) return "Missing policy for identity provider certificate signing key.";
        if ( identityProvider.mProviderKeyName != this->mGamercert.mKeyName ) return "Mismatched identity provider key names.";
        
        string fingerprint;
        
        TransactionResult identityResult = this->mGamercert.verify (
            identityProvider.mEd25519PublicHex,
            ledger.getIdentity (),
            CryptoKeyInfo ( context.mKeyAndPolicy.mKey.getKeyPair ()).mPublicKey,
            fingerprint
        );
        if ( !identityResult ) return identityResult;

        LedgerFieldODBM < bool > identityFingerprintField ( ledger, Ledger::keyFor_identityFingerprint ( this->mProvider, fingerprint ), false );
        bool firstFingerprint = ( identityFingerprintField.get () == false );

        u64 fees = this->getFees ();
        u64 grant = firstFingerprint ? identityProvider.mGrant : 0;
        
        if ( this->mGrant != grant ) return "Expected grant not matched.";
        if ( grant < fees ) return "Identity grant insufficient to cover transaction fees.";

        context.mAccountODBM.setIdentity ( fingerprint, this->mProvider );
        
        u64 net = grant - fees;
        if ( net > 0 ) {
            context.mAccountODBM.addFunds ( net );
        }
        
        if ( firstFingerprint ) {
            identityFingerprintField.set ( true );
        }
        
        if ( grant > 0 ) {
            ledger.createVOL ( 0, 0, grant );
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    u64 AbstractTransactionBody_getVOL ( const TransactionContext& context ) const override {
        UNUSED ( context );
    
        return this->mGrant;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
