// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FORMATLEDGERKEY_H
#define VOLITION_FORMATLEDGERKEY_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// LedgerKey
//================================================================//
class LedgerKey {
protected:

    friend class FormatLedgerKey;
    template < typename > friend class LedgerFieldODBM;

    string  mKey;

    //----------------------------------------------------------------//
    LedgerKey () {
    }

    //----------------------------------------------------------------//
    LedgerKey ( const char* key ) :
        mKey ( key ) {
    }

    //----------------------------------------------------------------//
    LedgerKey ( string key ) :
        mKey ( key ) {
    }

public:
    
    //----------------------------------------------------------------//
    operator const string () const {
        return this->mKey;
    }
};

//================================================================//
// FormatLedgerKey
//================================================================//
class FormatLedgerKey {
public:

    //----------------------------------------------------------------//
    // Account

    //----------------------------------------------------------------//
    static LedgerKey forAccount_assetCount ( Account::Index index ) {
        return Format::write ( "account.%d.assetCount", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_body ( Account::Index index ) {
        return Format::write ( "account.%d", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_inventoryField ( Account::Index index, size_t position ) {
        return Format::write ( "account.%d.assets.%d", index, position );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_minerInfo ( Account::Index index ) {
        return Format::write ( "account.%d.miner", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_name ( Account::Index index ) {
        return Format::write ( "account.%d.name", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_nonce ( Account::Index index ) {
        return Format::write ( "account.%d.nonce", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccount_transactionNoteField ( Account::Index index, u64 nonce ) {
        return Format::write ( "account.%d.transaction.%d", index, nonce );
    }

    //----------------------------------------------------------------//
    // Asset

    //----------------------------------------------------------------//
    static LedgerKey forAsset_owner ( Asset::Index index ) {
        return Format::write ( "asset.%d.owner", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAsset_position ( Asset::Index index ) {
        return Format::write ( "asset.%d.position", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAsset_type ( Asset::Index index ) {
        return Format::write ( "asset.%d.type", index );
    }

    //----------------------------------------------------------------//
    // Global

    //----------------------------------------------------------------//
    static LedgerKey forAccountAlias ( string accountName ) {
        assert ( accountName.size () > 0 );
        return Format::write ( "accountAlias.%s", accountName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccountKeyLookup ( string keyID ) {
        return Format::write ( "key.%s", keyID.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAssetModifiedField ( Asset::Index index, string fieldName ) {
        return Format::write ( "asset.%d.fields.%s", index, fieldName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forBlock () {
        return "block";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey forBlockSize () {
        return "blockSize";
    }

    //----------------------------------------------------------------//
    static LedgerKey forEntitlements ( string name ) {

        assert ( name.size () > 0 );
        return Format::write ( "entitlements.%s", name.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forEntropy () {
        return "entropy";
    }

    //----------------------------------------------------------------//
    static LedgerKey forGlobalAccountCount () {
        return "account.count";
    }

    //----------------------------------------------------------------//
    static LedgerKey forGlobalAssetCount () {
        return Format::write ( "asset.count" );
    }

    //----------------------------------------------------------------//
    static LedgerKey forIdentity () {
        return "identity";
    }

    //----------------------------------------------------------------//
    static LedgerKey forMiners () {
        return "miners";
    }

    //----------------------------------------------------------------//
    static LedgerKey forMinerURLs () {
        return "minerURLs";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey forSchema () {
        return "schema";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey forUnfinished () {
        return "unfinished";
    }
};

} // namespace Volition
#endif
