// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_RENAME_ACCOUNT_H
#define VOLITION_TRANSACTIONS_RENAME_ACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Policy.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RenameAccount
//================================================================//
class RenameAccount :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "RENAME_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string      mRevealedName;
    Digest      mNameHash;      // SHA256 ( "<new name>" )
    Digest      mNameSecret;    // SHA256 ( "<current name>:<new name>" )

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "revealedName",  this->mRevealedName );
        serializer.serialize ( "nameHash",      this->mNameHash );
        serializer.serialize ( "nameSecret",    this->mNameSecret );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
        
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::RENAME_ACCOUNT )) return "Permission denied.";
        
//        return context.mLedger.renameAccount ( this->mMaker->getAccountName (), this->mRevealedName, this->mNameHash, this->mNameSecret );
        
        // nameHash <- SHA256 ( "<new name>" )
        // nameSecret <- SHA256 ( "<current name>:<new name>" )

        // if provided, nameHash and nameSecret may be used to reduce the chances of an account
        // name being intercepted and registered by an attacker.

        // nameHash establishes the uniqueness of an account name without revealing it. nameSecret
        // binds the account name to the registrant's account. using both ensures that the registrant
        // really knows the requested account name. in other words, the registrant's own account
        // name acts like a salt: an attacker would have to find a match that produced both the
        // nameHash *and* the nameSecret (derived from their own account name). thus, nameHash
        // is shared, but nameSecret is unique to every account applying for the name.

        // the nameHash is used to create a decollider table, which records all accounts applying
        // for the name. the nameSecret is stored inside the account, along with a timestamp.

        // when a plaintext name is revealed, the decollider table is checked. each colliding
        // account may then be checked to see if its nameSecret is correct. whichever account
        // has the correct namesecret and the earliest timestamp is the rightful claimant
        // of the name.

        // once a claimant is found, the decollider table may be updated to reflect the change
        // in status and indicate the winner (although the winner must also submit a rename
        // transaction to claim the name).

        Ledger& ledger = context.mLedger;
        const Account& account = context.mAccount;

        string accountName       = context.mAccount.mName;
        string revealedName      = this->mRevealedName;

        // make sure the revealed name is valid
        if ( Ledger::isChildName ( revealedName )) return "Renamed accounts must not begin with '~'"; // new names must not begin with a '~'
        if ( !Ledger::isAccountName ( revealedName )) return "Proposed account name contains invalid characters."; // make sure it's a valid account name

        string lowerRevealedName = Format::tolower ( revealedName );
        LedgerFieldODBM < Account::Index > alias = LedgerFieldODBM < Account::Index > ( ledger, FormatLedgerKey::forAccountAlias ( lowerRevealedName ));
              
        // check to see if the alias already exists
        if ( alias.exists ()) {
            if ( alias.get () != account.mIndex ) return "Alias already exists (and belongs to another account.)";
            return "Alias already exists.";
        }
        alias.set ( account.mIndex );

        Account accountUpdated = account;
        accountUpdated.mName = revealedName;
        ledger.setObject < Account >( FormatLedgerKey::forAccount_body ( account.mIndex ), accountUpdated );
        ledger.setValue < string >( FormatLedgerKey::forAccount_name ( account.mIndex ), revealedName );
        
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
