// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/AccountODBM.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>
#include <volition/TransactionContext.h>
#include <volition/TransactionMaker.h>
#include <volition/transactions/Genesis.h>

namespace Volition {

//================================================================//
// Transaction
//================================================================//

//----------------------------------------------------------------//
TransactionResult Transaction::apply ( Ledger& ledger, time_t time, SchemaHandle& schemaHandle ) const {

    TransactionResult result = this->checkBody ( ledger, time );
    if ( result ) {
        result = this->applyInner ( ledger, schemaHandle );
    }
    result.setTransactionDetails ( *this );
    return result;
}

//----------------------------------------------------------------//
TransactionResult Transaction::applyInner ( Ledger& ledger, SchemaHandle& schemaHandle ) const {
    
    if ( !this->mBody ) return "Missing body.";
    
    shared_ptr < const Account > account;
    const KeyAndPolicy* keyAndPolicy = NULL;
    
    TransactionMaker* maker = this->mBody->mMaker.get ();
    if ( maker ) {
        
        account = ledger.getAccount ( ledger.getAccountIndex ( maker->getAccountName ()));
        if ( !account ) return "Transaction maker account not found.";
        
        keyAndPolicy = account->getKeyAndPolicyOrNull ( maker->getKeyName ());
        if ( !keyAndPolicy ) return "Transaction maker key not found.";
    }
    else if ( ledger.isGenesis ()) {
        const Transactions::Genesis* genesis = dynamic_cast < const Transactions::Genesis* >( this->mBody.get ());
        return genesis ? genesis->genesis ( ledger ) : TransactionResult ( "Missing transaction maker." );
    }
    
    TransactionResult result = this->checkNonceAndSignature ( ledger, *account, *keyAndPolicy );
    if ( result ) {
        
        TransactionContext context ( ledger, schemaHandle, *account, *keyAndPolicy );
        
        u64 cost = this->mBody->cost ();
        if ( account->mBalance < cost ) return "Insufficient funds.";
        
        result = this->mBody->apply ( context );
        
        if ( result ) {
            if ( !ledger.isGenesis ()) {
                
                ledger.incAccountTransactionNonce ( account->mIndex, maker->getNonce (), this->mBody->note ());
                
                if ( cost > 0 ) {
                    Account accountUpdated = *account;
                    accountUpdated.mBalance -= cost;
                    ledger.setAccount ( accountUpdated );
                }
            }
        }
    }
    return result;
}

//----------------------------------------------------------------//
TransactionResult Transaction::checkBody ( Ledger& ledger, time_t time ) const {

    if ( this->mBody->mNote.size () > MAX_NOTE_LENGTH ) return Format::write ( "Transaction note exceeds %d-character limit", MAX_NOTE_LENGTH );
    
    if ( this->mBody->mMaxHeight > 0 ) {
        u64 height = ledger.getHeight ();
        if ( height > this->mBody->mMaxHeight ) return Format::write ( "Transaction expired at chain height of %d.", this->mBody->mMaxHeight );
    }
    
    if ( this->mBody->mRecordBy > 0 ) {
        if ( time > this->mBody->mRecordBy ) return Format::write ( "Transaction expired at %s.", (( string )this->mBody->mRecordBy ).c_str ());
    }
    return true;
}

//----------------------------------------------------------------//
bool Transaction::checkMaker ( string accountName, u64 nonce ) const {

    if ( !this->mBody ) return false;
    if ( this->mBody->note ().size () == 0 ) return false;
    TransactionMaker* maker = this->mBody->mMaker.get ();
    return ( maker && ( maker->getAccountName () == accountName ) && ( maker->getNonce () == nonce ));
}

//----------------------------------------------------------------//
TransactionResult Transaction::checkNonceAndSignature ( const Ledger& ledger, const Account& account, const KeyAndPolicy& keyAndPolicy ) const {

    if ( ledger.isGenesis ()) return true;
    if ( this->mBody->note ().size () == 0 ) return false;

    TransactionMaker* maker = this->mBody->mMaker.get ();
    Signature* signature = this->mSignature.get ();

    if ( maker && signature ) {
        if ( !this->mBody->needsControl ()) {
            u64 nonce = ledger.getAccountTransactionNonce ( account.mIndex );
            if ( nonce != maker->getNonce ()) return false;
        }
        const CryptoKey& key = keyAndPolicy.mKey;
        return key.verify ( *signature, this->mBodyString );
    }
    return false;
}

//----------------------------------------------------------------//
TransactionResult Transaction::control ( Miner& miner, Ledger& ledger ) const {

    if ( !this->mBody->needsControl ()) return true;
    if ( !this->mBody ) return "CONTROL: Missing body.";
    if ( !miner.controlPerimitted ()) return "CONTROL: Control not permitted by this mining node.";
    
    TransactionMaker* maker = this->mBody->mMaker.get ();
    shared_ptr < const Account > account = ledger.getAccount ( ledger.getAccountIndex ( maker->getAccountName ()));
    const KeyAndPolicy* keyAndPolicy = account->getKeyAndPolicyOrNull ( maker->getKeyName ());

    TransactionResult result = this->checkNonceAndSignature ( ledger, *account, *keyAndPolicy );
    if ( !result ) return "CONTROL: Invalid account or signature.";
        
    Entitlements entitlements = ledger.getEntitlements < KeyEntitlements >( *keyAndPolicy );
    if ( !entitlements.check ( KeyEntitlements::NODE_CONTROL )) return "Permission denied.";
    
    result = this->mBody->control ( miner, ledger );
    
    if ( result ) {
        AccountODBM accountODBM ( ledger, account->mIndex );
        accountODBM.mTransactionNonce.set ( maker->getNonce ()); // updated the nonce so the *next* transaction will go through
        // TODO: recover account if destroyed by control
    }
    return result;
}

//----------------------------------------------------------------//
u64 Transaction::getGratuity () const {

    return this->mBody ? this->mBody->gratuity () : 0;
}

//----------------------------------------------------------------//
const TransactionMaker* Transaction::getMaker () const {

    return ( this->mBody && this->mBody->mMaker ) ? this->mBody->mMaker.get () : NULL;
}

//----------------------------------------------------------------//
string Transaction::getNote () const {

    return this->mBody ? this->mBody->note () : "";
}


//----------------------------------------------------------------//
u64 Transaction::maturity () const {
    return this->mBody->maturity ();
}

//----------------------------------------------------------------//
Transaction::Transaction () {
}

//----------------------------------------------------------------//
Transaction::~Transaction () {
}

//----------------------------------------------------------------//
void Transaction::setBody ( shared_ptr < AbstractTransactionBody > body ) {

    this->mBody = body;
    this->mBodyString = body ? ToJSONSerializer::toJSONString ( *body ) : "";
}

//----------------------------------------------------------------//
u64 Transaction::weight () const {
    return this->mBody->weight ();
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Transaction::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "signature", this->mSignature );
    
    if ( serializer.has ( "bodyIn" )) {
        TransactionBodyPtr body;
        serializer.serialize ( "bodyIn", body );
        this->setBody ( body );
    }
    else {
        serializer.serialize ( "body", this->mBodyString );
        if ( this->mBodyString.size ()) {
        
            FromJSONSerializer::fromJSONString ( this->mBody, this->mBodyString );
            assert ( this->mBody );
        }
    }
}

//----------------------------------------------------------------//
void Transaction::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "signature",     this->mSignature );
    serializer.serialize ( "body",          this->mBodyString );
}

} // namespace Volition
