// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETODBM_H
#define VOLITION_ASSETODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// AssetODBM
//================================================================//
class AssetODBM {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryNonce ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "asset.%d.inventoryNonce", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_owner ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "asset.%d.owner", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_position ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "asset.%d.position", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_type ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "asset.%d.type", index ); });
    }

public:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_field ( AssetID::Index index, string fieldName ) {
        return Format::write ( "asset.%d.fields.%s", index, fieldName.c_str ());
    }

    ConstOpt < Ledger >     mLedger;
    AssetID                 mAssetID;

    LedgerFieldODBM < AccountID::Index >    mOwner;
    LedgerFieldODBM < u64 >                 mInventoryNonce;
    LedgerFieldODBM < u64 >                 mPosition;
    LedgerFieldODBM < string >              mType;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mOwner.exists ();
    }

    //----------------------------------------------------------------//
    AssetODBM ( ConstOpt < Ledger > ledger, AssetID::Index index ) :
        mLedger ( ledger ),
        mAssetID ( index ),
        mOwner ( ledger,            keyFor_owner ( this->mAssetID ),            AccountID::NULL_INDEX ),
        mInventoryNonce ( ledger,   keyFor_inventoryNonce ( this->mAssetID ),   0 ),
        mPosition ( ledger,         keyFor_position ( this->mAssetID ),         0 ),
        mType ( ledger,             keyFor_type ( this->mAssetID ),             "" ) {
    }
    
    //----------------------------------------------------------------//
    shared_ptr < const Asset > getAsset ( const Schema& schema, bool sparse = false ) {

        if ( !this->mOwner.exists ()) return NULL;

        const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( this->mType.get ());
        if ( !assetDefinition ) return NULL;
        
        AccountID ownerAccountIndex = this->mOwner.get ();
        AccountODBM ownerODBM ( this->mLedger, ownerAccountIndex );
        
        shared_ptr < Asset > asset = make_shared < Asset >();
        asset->mType            = this->mType.get ();
        asset->mAssetID         = this->mAssetID;
        asset->mOwner           = ownerODBM.mName.get ();
        asset->mInventoryNonce  = this->mInventoryNonce.get ( 0 );
        
        // copy the fields and apply any overrides
        AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
        for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
            
            string fieldName = fieldIt->first;
            
            const AssetFieldDefinition& field = fieldIt->second;
            AssetFieldValue value = field;
            
            if ( field.mMutable  ) {

                LedgerKey KEY_FOR_ASSET_FIELD = keyFor_field ( this->mAssetID, fieldName );

                switch ( field.getType ()) {
                
                    case AssetFieldValue::Type::TYPE_BOOL:
                        value = LedgerFieldODBM < bool >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( value.strictBoolean ());
                        break;
                        
                    case AssetFieldValue::Type::TYPE_NUMBER:
                        value = LedgerFieldODBM < double >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( value.strictNumber ());
                        break;
                        
                    case AssetFieldValue::Type::TYPE_STRING:
                        value = LedgerFieldODBM < string >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( value.strictString ());
                        break;
                        
                    default:
                        break;
                }
            }
            
            if (( sparse == false ) || ( value != field )) {
                asset->mFields [ fieldName ] = value;
            }
        }
        return asset;
    }
};

} // namespace Volition
#endif
