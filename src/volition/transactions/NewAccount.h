// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_NEWACCOUNT_H
#define VOLITION_TRANSACTIONS_NEWACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/ContractWithDigest.h>
#include <volition/Gamercert.h>
#include <volition/IdentityProvider.h>
#include <volition/Signature.h>

namespace Volition {
namespace Transactions {

//================================================================//
// NewAccount
//================================================================//
class NewAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "NEW_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )
    TRANSACTION_MIN_RELEASE ( 3 )

    string              mAccountName;
    string              mProvider;
    string              mGenesis;
    CryptoPublicKey     mKey;
    Gamercert           mGamercert;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",       this->mAccountName );
        serializer.serialize ( "provider",          this->mProvider );
        serializer.serialize ( "genesis",           this->mGenesis );
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "identity",          this->mGamercert );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",       this->mAccountName );
        serializer.serialize ( "provider",          this->mProvider );
        serializer.serialize ( "genesis",           this->mGenesis );
        serializer.serialize ( "key",               this->mKey );
        serializer.serialize ( "identity",          this->mGamercert );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( this->wasApplied ( context.mLedger )) return "Key already used to create an account.";
        
        AbstractLedger& ledger = context.mLedger;
        
        if ( this->mAccountName.size () > 0 ) {
             if ( ledger.getAccountID ( this->mAccountName ) != AccountID::NULL_INDEX ) return "An account with that name already exists.";
        }
        
        if ( this->mGenesis != ledger.getGenesisHash ()) return "Mismatched genesis block hash.";
        
        IdentityProvider identityProvider = ledger.getIdentityProvider ( this->mProvider );
        if ( !identityProvider ) return "Missing policy for identity provider certificate signing key.";
        if ( identityProvider.mProviderKeyName != this->mGamercert.mKeyName ) return "Mismatched identity provider key names.";
        
        string fingerprint;
        
        TransactionResult identityResult = this->mGamercert.verify (
            identityProvider.mEd25519PublicHex,
            context.mLedger.getIdentity (),
            CryptoKeyInfo ( this->mKey.getKeyPair ()).mPublicKey,
            fingerprint
        );
        if ( !identityResult ) return identityResult;
        
        LedgerFieldODBM < bool > identityFingerprintField ( ledger, Ledger::keyFor_identityFingerprint ( this->mProvider, fingerprint ), false );
        bool firstFingerprint = ( identityFingerprintField.get () == false );
        
        u64 fees = this->getFees ();
        u64 grant = firstFingerprint ? identityProvider.mGrant : 0;
        
        if ( grant < fees ) return "Identity grant insufficient to cover transaction fees.";
        
        string accountName = ( this->mAccountName.size () > 0 ) ? this->mAccountName : Format::write ( ".%llu.%llu", context.mBlockHeight, context.mIndex );
        if ( !ledger.newAccount (
            accountName,
            0,
            Ledger::MASTER_KEY_NAME,
            this->mKey,
            identityProvider.mKeyPolicy,
            identityProvider.mAccountPolicy
        )) {
            return "Failed to create account.";
        }
        
        AccountODBM accountODBM ( ledger, accountName );
        accountODBM.setIdentity ( fingerprint, this->mProvider );
        
        u64 balance = grant - fees;
        if ( balance > 0 ) {
            accountODBM.mBalance.set ( balance );
        }
        
        if ( firstFingerprint ) {
            identityFingerprintField.set ( true );
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_postApply ( TransactionContext& context ) const override {
        
        context.mLedger.setValue < bool >( Ledger::keyFor_makerlessTransactionID ( this->getTypeString (), this->mKey.getKeyID ()), true );
        return true;
    }

    //----------------------------------------------------------------//
    bool AbstractTransactionBody_wasApplied ( const AbstractLedger& ledger ) const override {
        
        return ledger.getValueOrFallback < bool >( Ledger::keyFor_makerlessTransactionID ( this->getTypeString (), this->mKey.getKeyID ()), false );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
