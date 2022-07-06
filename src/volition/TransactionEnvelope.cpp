// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransaction.h>
#include <volition/AccountODBM.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/TransactionEnvelope.h>
#include <volition/TransactionContext.h>
#include <volition/TransactionMaker.h>
#include <volition/transactions/Genesis.h>

namespace Volition {

//================================================================//
// Transaction
//================================================================//

//----------------------------------------------------------------//
TransactionResult TransactionEnvelope::apply ( AbstractLedger& ledger, u64 blockHeight, u64 index, time_t time, Block::VerificationPolicy policy ) const {

    try {
        const TransactionMaker& maker = this->getMaker ();
    
        TransactionContext context ( ledger, maker, blockHeight, index, time );
        TransactionResult result = this->checkSignature ( context, policy );
        if ( result ) {
            result = this->mBody->apply ( context );
        }
        result.setTransactionDetails ( *this );
        return result;
    }
    catch ( const std::exception &e ) {
        return Format::write ( "Exception occured while applying transaction: %s", e.what ());
    }
    catch ( ... ) {
        return "Exception occured while applying transaction.";
    }
}

//----------------------------------------------------------------//
bool TransactionEnvelope::checkMaker ( string accountName, string uuid ) const {

    if ( !this->mBody ) return false;
    if ( this->getUUID ().size () == 0 ) return false;
    return (( this->getMakerAccountName () == accountName ) && ( this->getUUID () == uuid ));
}

//----------------------------------------------------------------//
TransactionResult TransactionEnvelope::checkSignature ( const TransactionContext& context, Block::VerificationPolicy policy ) const {

    if ( !( policy & Block::VERIFY_TRANSACTION_SIG )) return true;

    if ( context.isGenesis ()) return true;
    if ( context.isProvisional ()) return true;
    
    Signature* signature = this->mSignature.get ();
    if ( !context.mKeyAndPolicy.mKey.verify ( *signature, this->mBodyString )) return "Invalid transaction signature.";
    
    return true;
}

//----------------------------------------------------------------//
TransactionDetailsPtr TransactionEnvelope::getDetails ( const AbstractLedger& ledger ) const {

    return this->mBody ? this->mBody->getDetails ( ledger ) : NULL;
}

//----------------------------------------------------------------//
void TransactionEnvelope::setBody ( shared_ptr < AbstractTransaction > body ) {

    this->mBody = body;
    this->mBodyString = body ? ToJSONSerializer::toJSONString ( *body ) : "";
}

//----------------------------------------------------------------//
void TransactionEnvelope::sign ( const CryptoKeyPair& keyPair ) {

    this->mSignature = make_shared < Signature >();
    *this->mSignature = keyPair.sign ( this->mBodyString );
}

//----------------------------------------------------------------//
TransactionEnvelope::TransactionEnvelope () {
}

//----------------------------------------------------------------//
TransactionEnvelope::~TransactionEnvelope () {
}

//----------------------------------------------------------------//
bool TransactionEnvelope::wasApplied ( const AbstractLedger& ledger ) const {

    return this->mBody ? this->mBody->wasApplied ( ledger ) : false;
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void TransactionEnvelope::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

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
void TransactionEnvelope::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "signature",     this->mSignature );
    serializer.serialize ( "body",          this->mBodyString );
}

} // namespace Volition
