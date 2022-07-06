// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RESEREACCOUNTNAME_H
#define VOLITION_TRANSACTIONS_RESEREACCOUNTNAME_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AccountODBM.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// ReserveAccountName
//================================================================//
class ReserveAccountName :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "RESERVE_ACCOUNT_NAME" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    Digest      mNameHash;      // SHA256 ( "<new name>" )
    Digest      mNameSecret;    // SHA256 ( "<account ID>:<new name>" )

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        // nameHash <- SHA256 ( "<new name>" )
        // nameSecret <- SHA256 ( "<account ID>:<new name>" )

        // if provided, nameHash and nameSecret may be used to reduce the chances of an account
        // name being intercepted and registered by an attacker.

        // nameHash establishes the uniqueness of an account name without revealing it. nameSecret
        // binds the account name to the registrant's account. using both ensures that the registrant
        // really knows the requested account name. in other words, the registrant's own account
        // name acts like a salt: an attacker would have to find a match that produced both the
        // nameHash *and* the nameSecret (derived from their own account name). thus, nameHash
        // is shared, but nameSecret is unique to every account applying for the name.

        // the nameHash is used to create a collision table, which records all accounts applying
        // for the name. the collision table includes the nameSecret for each collision.

        // when a plaintext name is revealed, the collision table is checked. the nameSecret
        // is generated, and each collider is checked. the one that matches wins.

        // SCENARIO 1:
        // 1. A plaintext name is submitted. It is hashed and checked for collisions (hashed and unhashed).
        // If no collisions, it is awarded.

        // SCENARIO 2:
        // 1. A nameHash and nameSecret are submitted. The nameSecret is stored in the collision table.
        
        // 2. A plaintext name is submitted. Is is hashed and used to look up the collision table.
        // The nameSecret is also generated. It is used to verify ownership of the name: if the
        // generated nameSecret is found in the collision table, the name is awarded.

        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RESERVE_ACCOUNT_NAME )) return "Permission denied.";

        if (( this->mNameHash.size () > 0 ) && ( this->mNameSecret.size () > 0 )) {
            return context.mLedger.reserveAccountname ( this->mNameHash, this->mNameSecret );
        }
        return "Missing parameters.";
    }
};

} // namespace Transactions
} // namespace Volition
#endif
