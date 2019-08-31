// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FORMATLEDGERKEY_H
#define VOLITION_FORMATLEDGERKEY_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// LedgerKey
//================================================================//
class LedgerKey {
private:

    friend class FormatLedgerKey;

    string  mKey;

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
    operator string () {
        return this->mKey;
    }
};

//================================================================//
// FormatLedgerKey
//================================================================//
class FormatLedgerKey {
private:

    static constexpr const char* ACCOUNT            = "account";
    static constexpr const char* ACCOUNT_ALIAS      = "alias";
    static constexpr const char* ACCOUNT_COUNTER    = "accountCounter";
    static constexpr const char* BLOCK_KEY          = "block";
    static constexpr const char* KEY_ID             = "keyID.";
    static constexpr const char* ENTROPY            = "entropy";
    static constexpr const char* IDENTITY           = "identity";
    static constexpr const char* MINERS             = "miners";
    static constexpr const char* MINER_INFO         = "minerInfo";
    static constexpr const char* MINER_URLS         = "minerUrls";
    static constexpr const char* UNFINISHED         = "unfinished";

public:

    static constexpr const char* ACCOUNT_NAME           = "name";
    static constexpr const char* ACCOUNT_HEAD           = "head";
    static constexpr const char* ACCOUNT_TAIL           = "tail";
    static constexpr const char* ACCOUNT_MINER_INFO     = "miner";

    static constexpr const char* ASSET_NEXT             = "next";
    static constexpr const char* ASSET_OWNER            = "owner";
    static constexpr const char* ASSET_PREV             = "prev";
    static constexpr const char* ASSET_TYPE             = "type";

    //----------------------------------------------------------------//
    static LedgerKey forAccount ( Account::Index index ) {

        return Format::write ( "account.%d", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccountAlias ( string accountName ) {

        assert ( accountName.size () > 0 );
        return Format::write ( "accountAlias.%s", accountName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccountCount () {

        return "account.count";
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccountKeyLookup ( string keyID ) {

        return Format::write ( "key.%s", keyID.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAccountMember ( Account::Index index, string member ) {

        assert ( member.size () > 0 );
        return Format::write ( "account.%d.%s", index, member.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAssetCount () {

        return Format::write ( "asset.count" );
    }

    //----------------------------------------------------------------//
    static LedgerKey forAssetDefinition ( string assetType ) {

        return Format::write ( "asset.%s.definition", assetType.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAssetField ( Asset::Index index, string fieldName ) {

        return Format::write ( "asset.%d.fields.%s", index, fieldName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forAssetMember ( Asset::Index index, string member ) {

        assert ( member.size () > 0 );
        return Format::write ( "asset.%d.%s", index, member.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forBlock () {

        return "block";
    }

    //----------------------------------------------------------------//
    static LedgerKey forEntropy () {

        return "entropy";
    }

    //----------------------------------------------------------------//
    static LedgerKey forIdentity () {

        return "identity";
    }

    //----------------------------------------------------------------//
    static LedgerKey forMethod ( string methodName ) {

        return Format::write ( "method.%s", methodName.c_str ());
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
    static LedgerKey forSchema ( Schema::Index index ) {

        return Format::write ( "schema.%d", index );
    }
    
    //----------------------------------------------------------------//
    static LedgerKey forSchemaAlias ( string schemaName ) {

        return Format::write ( "schemaAlias.%s", schemaName.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey forSchemaCount () {

        return "schema.count";
    }
    
    //----------------------------------------------------------------//
    static LedgerKey forUnfinished () {

        return "unfinished";
    }
};

} // namespace Volition
#endif
