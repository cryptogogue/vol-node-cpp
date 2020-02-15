// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGERODBM_H
#define VOLITION_LEDGERODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// ConstOption
//================================================================//
template < typename TYPE >
class ConstOpt {
private:

    const TYPE*     mConst;
    TYPE*           mMutable;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mConst != NULL );
    }

    //----------------------------------------------------------------//
    ConstOpt () :
        mConst ( NULL ),
        mMutable ( NULL ) {
    }

    //----------------------------------------------------------------//
    ConstOpt ( const TYPE& value ) :
        mConst ( &value ),
        mMutable ( NULL ) {
    }
    
    //----------------------------------------------------------------//
    ConstOpt ( TYPE& value ) :
        mConst ( &value ),
        mMutable ( &value ) {
    }
    
    //----------------------------------------------------------------//
    const TYPE& get () const {
        return this->getConst ();
    }
    
    //----------------------------------------------------------------//
    TYPE& get () {
        return this->getMutable ();
    }
    
    //----------------------------------------------------------------//
    const TYPE& getConst () const {
        assert ( this->mConst );
        return *this->mConst;
    }
    
    //----------------------------------------------------------------//
    TYPE& getMutable () {
        assert ( this->mMutable );
        return *this->mMutable;
    }
    
    //----------------------------------------------------------------//
    void set ( const TYPE& value ) {
        this->mConst = &value;
        this->mMutable = NULL;
    }
    
    //----------------------------------------------------------------//
    void set ( TYPE& value ) {
        this->mConst = &value;
        this->mMutable = &value;
    }
    
    //----------------------------------------------------------------//
    bool isConst () {
        return ( this->mMutable == NULL );
    }

    //----------------------------------------------------------------//
    bool isMutable () {
        return ( this->mMutable != NULL );
    }
};

//================================================================//
// LedgerFieldODBM
//================================================================//
template < typename TYPE >
class LedgerFieldODBM {
private:

    ConstOpt < Ledger >     mLedger;
    LedgerKey               mKey;
    bool                    mIsLoaded;
    TYPE                    mValue;

public:

    //----------------------------------------------------------------//
    bool exists () const {
            
        return this->mLedger.getConst ().hasValue ( this->mKey );
    }

    //----------------------------------------------------------------//
    TYPE get () {
        
        if ( !this->mIsLoaded ) {
            this->mValue = this->mLedger.getConst ().template getValue < TYPE >( this->mKey );
            this->mIsLoaded = true;
        }
        return this->mValue;
    }
    
    //----------------------------------------------------------------//
    TYPE get ( const TYPE& fallback ) {
        
        if ( !this->mIsLoaded ) {
            this->mValue = this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, fallback );
            this->mIsLoaded = true;
        }
        return this->mValue;
    }

    //----------------------------------------------------------------//
    void init ( const Ledger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }
    
    //----------------------------------------------------------------//
    void init ( Ledger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }

    //----------------------------------------------------------------//
    void set ( const TYPE& value ) {
        
        if ( !( this->mIsLoaded && ( this->mValue == value ))) {
            this->mLedger.getMutable ().template setValue < TYPE >( this->mKey, value );
            this->mValue = value;
            this->mIsLoaded = true;
        }
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM () :
        mIsLoaded ( false ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < Ledger > ledger, const LedgerKey& ledgerKey ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mIsLoaded ( false ) {
    }
};

//================================================================//
// AccountODBM
//================================================================//
class AccountODBM {
public:

    ConstOpt < Ledger >     mLedger;
    Account::Index          mIndex;

    LedgerFieldODBM < string >      mBody;
    LedgerFieldODBM < size_t >      mAssetCount;
    LedgerFieldODBM < string >      mMinerInfo;
    LedgerFieldODBM < string >      mName;

    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < Ledger > ledger, Account::Index index ) :
        mIndex ( index ),
        mLedger ( ledger ),
        mBody ( ledger,          FormatLedgerKey::forAccount_body ( this->mIndex )),
        mAssetCount ( ledger,    FormatLedgerKey::forAccount_assetCount ( this->mIndex )),
        mMinerInfo ( ledger,     FormatLedgerKey::forAccount_minerInfo ( this->mIndex )),
        mName ( ledger,          FormatLedgerKey::forAccount_name ( this->mIndex )) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < Asset::Index > getInventoryField ( size_t position ) {
    
        return LedgerFieldODBM < Asset::Index >( this->mLedger, FormatLedgerKey::forAccount_inventoryField ( this->mIndex, position ));
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < string > getTransactionNoteField ( u64 nonce ) {
    
        return LedgerFieldODBM < string >( this->mLedger, FormatLedgerKey::forAccount_transactionNoteField ( this->mIndex, nonce ));
    }
};

//================================================================//
// AssetODBM
//================================================================//
class AssetODBM {
public:

    ConstOpt < Ledger >     mLedger;
    Asset::Index            mIndex;

    LedgerFieldODBM < Asset::Index >    mOwner;
    LedgerFieldODBM < size_t >          mPosition;
    LedgerFieldODBM < string >          mType;

    //----------------------------------------------------------------//
    AssetODBM ( ConstOpt < Ledger > ledger, Asset::Index index ) :
        mLedger ( ledger ),
        mIndex ( index ),
        mOwner ( ledger,        FormatLedgerKey::forAsset_owner ( this->mIndex )),
        mPosition ( ledger,     FormatLedgerKey::forAsset_position ( this->mIndex )),
        mType ( ledger,         FormatLedgerKey::forAsset_type ( this->mIndex )) {
    }
};

} // namespace Volition
#endif
