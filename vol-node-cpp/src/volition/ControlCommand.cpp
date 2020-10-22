// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/AccountODBM.h>
#include <volition/ControlCommand.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/TransactionContext.h>
#include <volition/TransactionMaker.h>
#include <volition/transactions/Genesis.h>

namespace Volition {

//================================================================//
// Transaction
//================================================================//

//----------------------------------------------------------------//
ControlCommand::ControlCommand () {
}

//----------------------------------------------------------------//
ControlCommand::~ControlCommand () {
}

//----------------------------------------------------------------//
Miner::Control ControlCommand::controlLevel () const {

    return this->mBody ? this->mBody->controlLevel () : Miner::CONTROL_NONE;
}

//----------------------------------------------------------------//
LedgerResult ControlCommand::execute ( Miner& miner ) const {

    if ( !this->mBody ) return "Invalid control command.";

    if ( miner.getControlLevel () < this->controlLevel ()) return "Unuathorized: control level.";

    Signature* signature = this->mSignature.get ();
    if ( !( signature && miner.getControlKey ().verify ( *signature, this->mBodyString ))) return "Signature error.";

    return this->mBody->AbstractControlCommandBody_execute ( miner );
}

//----------------------------------------------------------------//
void ControlCommand::setBody ( shared_ptr < AbstractControlCommandBody > body ) {

    this->mBody = body;
    this->mBodyString = body ? ToJSONSerializer::toJSONString ( *body ) : "";
}

//----------------------------------------------------------------//
string ControlCommand::typeString () const {

    return this->mBody ? this->mBody->typeString () : "";
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void ControlCommand::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "signature", this->mSignature );
    
    if ( serializer.has ( "bodyIn" )) {
        CommandBodyPtr body;
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
void ControlCommand::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "signature",     this->mSignature );
    serializer.serialize ( "body",          this->mBodyString );
}

} // namespace Volition
