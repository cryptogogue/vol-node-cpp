// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Transaction.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Transaction
//================================================================//

//----------------------------------------------------------------//
bool Transaction::apply ( Ledger& ledger, SchemaHandle& schemaHandle ) const {
    
    if ( this->checkMaker ( ledger )) {
        if ( this->mBody->apply ( ledger, schemaHandle )) {
            if ( this->mBody->mMaker ) {
                ledger.incrementNonce ( *this->mBody->mMaker, this->mBody->note ());
            }
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Transaction::checkMaker ( const Ledger& ledger ) const {

    if ( !this->mBody ) return false;
    if ( ledger.isGenesis ()) return true;
    if ( this->mBody->note ().size () == 0 ) return false;

    TransactionMaker* maker = this->mBody->mMaker.get ();
    Signature* signature = this->mSignature.get ();

    if ( maker && signature ) {
        
        AccountKey accountKey = ledger.getAccountKey ( maker->getAccountName (), maker->getKeyName ());
        if ( accountKey ) {
        
            if ( accountKey.mAccount->getNonce () != maker->getNonce ()) return false;

            const CryptoKey& key = accountKey.mKeyAndPolicy->mKey;
            return key.verify ( *signature, this->mBodyString );
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Transaction::checkMaker ( string accountName, u64 nonce ) const {

    if ( !this->mBody ) return false;
    if ( this->mBody->note ().size () == 0 ) return false;
    TransactionMaker* maker = this->mBody->mMaker.get ();
    return ( maker && ( maker->getAccountName () == accountName ) && ( maker->getNonce () == nonce ));
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
