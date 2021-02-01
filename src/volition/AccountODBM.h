// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTODBM_H
#define VOLITION_ACCOUNTODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// AccountODBM
//================================================================//
class AccountODBM {
public:

    ConstOpt < Ledger >     mLedger;
    Account::Index          mIndex;

    LedgerFieldODBM < string >      mBody;
    LedgerFieldODBM < u64 >         mAssetCount;
    LedgerFieldODBM < u64 >         mInventoryNonce;
    LedgerFieldODBM < string >      mMinerInfo;
    LedgerFieldODBM < u64 >         mTransactionNonce;
    LedgerFieldODBM < string >      mName;

    //----------------------------------------------------------------//
    static LedgerKey keyFor_assetCount ( Account::Index index ) {
        return Format::write ( "account.%d.assetCount", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_body ( Account::Index index ) {
        return Format::write ( "account.%d", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryField ( Account::Index index, size_t position ) {
        return Format::write ( "account.%d.assets.%d", index, position );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryLogEntry ( Account::Index index, u64 inventoryNonce ) {
        return Format::write ( "account.%d.inventoryLog.%d", index, inventoryNonce );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryNonce ( Account::Index index ) {
        return Format::write ( "account.%d.inventoryNonce", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerInfo ( Account::Index index ) {
        return Format::write ( "account.%d.miner", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_name ( Account::Index index ) {
        return Format::write ( "account.%d.name", index );
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionLookup ( Account::Index index, string uuid ) {
       return Format::write ( "account.%d.transactionLookup.%s", index, uuid.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionNonce ( Account::Index index ) {
        return Format::write ( "account.%d.transactionNonce", index );
    }

    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < Ledger > ledger, Account::Index index ) :
        mLedger ( ledger ),
        mIndex ( index ),
        mBody ( ledger,                 keyFor_body ( this->mIndex )),
        mAssetCount ( ledger,           keyFor_assetCount ( this->mIndex )),
        mInventoryNonce ( ledger,       keyFor_inventoryNonce ( this->mIndex )),
        mMinerInfo ( ledger,            keyFor_minerInfo ( this->mIndex )),
        mTransactionNonce ( ledger,     keyFor_transactionNonce ( this->mIndex )),
        mName ( ledger,                 keyFor_name ( this->mIndex )) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < AssetID::Index > getInventoryField ( size_t position ) {
    
        return LedgerFieldODBM < AssetID::Index >( this->mLedger, keyFor_inventoryField ( this->mIndex, position ));
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < u64 > getTransactionLookupField ( string uuid ) {
    
        return LedgerFieldODBM < u64 >( this->mLedger, keyFor_transactionLookup ( this->mIndex, uuid ));
    }
};

} // namespace Volition
#endif
