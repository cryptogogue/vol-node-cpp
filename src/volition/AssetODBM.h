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
    static LedgerKey keyFor_offer ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "asset.%d.offer", index ); });
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
    LedgerKey keyFor_field ( string fieldName ) {
        return keyFor_field ( this->mAssetID, fieldName );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_field ( AssetID::Index index, string fieldName ) {
        return Format::write ( "asset.%d.fields.%s", index, fieldName.c_str ());
    }

    ConstOpt < AbstractLedger >             mLedger;
    AssetID                                 mAssetID;

    LedgerFieldODBM < AccountID::Index >    mOwner;
    LedgerFieldODBM < u64 >                 mInventoryNonce;
    LedgerFieldODBM < u64 >                 mPosition;
    LedgerFieldODBM < string >              mType;
    LedgerFieldODBM < OfferID::Index >      mOffer;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mOwner.exists ();
    }

    //----------------------------------------------------------------//
    AssetODBM ( ConstOpt < AbstractLedger > ledger, AssetID::Index index ) :
        mLedger ( ledger ),
        mAssetID ( index ),
        mOwner ( ledger,            keyFor_owner ( this->mAssetID ),            AccountID::NULL_INDEX ),
        mInventoryNonce ( ledger,   keyFor_inventoryNonce ( this->mAssetID ),   0 ),
        mPosition ( ledger,         keyFor_position ( this->mAssetID ),         0 ),
        mType ( ledger,             keyFor_type ( this->mAssetID ),             "" ),
        mOffer ( ledger,            keyFor_offer ( this->mAssetID ),            OfferID::NULL_INDEX ) {
    }
    
    //----------------------------------------------------------------//
    shared_ptr < const Asset > getAsset ( bool sparse = false ) {

        if ( !this->mOwner.exists ()) return NULL;

        const Schema& schema = this->mLedger.getConst ().getSchema ();

        const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( this->mType.get ());
        if ( !assetDefinition ) return NULL;
        
        AccountID ownerAccountIndex = this->mOwner.get ();
        AccountODBM ownerODBM ( this->mLedger, ownerAccountIndex );
        
        shared_ptr < Asset > asset = make_shared < Asset >();
        asset->mType            = this->mType.get ();
        asset->mAssetID         = this->mAssetID;
        asset->mOwner           = ownerODBM.mName.get ();
        asset->mInventoryNonce  = this->mInventoryNonce.get ( 0 );
        asset->mOfferID         = this->mOffer.get ();
        
        // copy the fields and apply any overrides
        AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
        for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
        
            string fieldName = fieldIt->first;
            AssetFieldValue value = this->getFieldValue ( fieldName, *assetDefinition, sparse );
            
            if ( value.isValid ()) {
                asset->mFields [ fieldName ] = value;
            }
        }
        return asset;
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue getFieldValue ( string fieldName, const AssetDefinition& assetDefinition, bool sparse = false ) {

        const AssetFieldDefinition& fieldDefinition = assetDefinition.getField ( fieldName );
        if ( !fieldDefinition.isValid ()) return AssetFieldValue ();
        if ( !fieldDefinition.mMutable ) return fieldDefinition;
        
        AssetFieldValue value;
        LedgerKey KEY_FOR_ASSET_FIELD = keyFor_field ( this->mAssetID, fieldName );
        
        switch ( fieldDefinition.getType ()) {
        
            case AssetFieldValue::Type::TYPE_BOOL:
                value = LedgerFieldODBM < bool >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( fieldDefinition.strictBoolean ());
                break;
                
            case AssetFieldValue::Type::TYPE_NUMBER:
                value = LedgerFieldODBM < double >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( fieldDefinition.strictNumber ());
                break;
                
            case AssetFieldValue::Type::TYPE_STRING:
                value = LedgerFieldODBM < string >( this->mLedger, KEY_FOR_ASSET_FIELD ).get ( fieldDefinition.strictString ());
                break;
                
            default:
                break;
        }
        
        if (( sparse == false ) || ( value != fieldDefinition )) return value;
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue getFieldValue ( string fieldName, bool sparse = false ) {

        AssetFieldValue value;

        if ( !this->mOwner.exists ()) return AssetFieldValue ();

        const Schema& schema = this->mLedger.getConst ().getSchema ();

        const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( this->mType.get ());
        if ( !assetDefinition ) return AssetFieldValue ();
        
        return this->getFieldValue ( fieldName, sparse );
    }
    
    //----------------------------------------------------------------//
    LedgerResult resetFields () {

        if ( !this->mOwner.exists ()) return false;

        const Schema& schema = this->mLedger.getConst ().getSchema ();

        const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( this->mType.get ());
        if ( !assetDefinition ) return false;
        
        // copy the fields and apply any overrides
        AssetDefinition::Fields::const_iterator fieldIt = assetDefinition->mFields.cbegin ();
        for ( ; fieldIt != assetDefinition->mFields.cend (); ++fieldIt ) {
            
            const AssetFieldDefinition& field = fieldIt->second;
            if ( !field.mMutable ) continue;
            
            this->setFieldValue ( fieldIt->first, fieldIt->second );
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    LedgerResult setFieldValue ( string fieldName, const AssetDefinition& assetDefinition, const AssetFieldValue& fieldValue ) {
    
        AssetFieldDefinition fieldDefinition = assetDefinition.getField ( fieldName );
        if ( !fieldDefinition.mMutable ) return Format::write ( "Field '%s' is not mutable.", fieldName.c_str ());
        if ( fieldDefinition.getType () != fieldValue.getType ())  return Format::write ( "Type mismatch on field '%s'.", fieldName.c_str ());
    
        AssetFieldValue currentValue = this->getFieldValue ( fieldName, assetDefinition );
        if ( !currentValue.isValid ()) {
            currentValue = fieldDefinition;
        }
        
        if ( currentValue != fieldValue ) {
    
            LedgerKey KEY_FOR_ASSET_FIELD = this->keyFor_field ( fieldName );
            
            switch ( fieldValue.getType ()) {
            
                case AssetFieldValue::Type::TYPE_BOOL:
                    this->mLedger->setValue < bool >( KEY_FOR_ASSET_FIELD, fieldValue.strictBoolean ());
                    break;
                    
                case AssetFieldValue::Type::TYPE_NUMBER:
                    this->mLedger->setValue < double >( KEY_FOR_ASSET_FIELD, fieldValue.strictNumber ());
                    break;
                    
                case AssetFieldValue::Type::TYPE_STRING:
                    this->mLedger->setValue < string >( KEY_FOR_ASSET_FIELD, fieldValue.strictString ());
                    break;
                    
                default:
                    return "Unknown or invalid param type.";
            }
        }
        return true;
    }
    
    //----------------------------------------------------------------//
    LedgerResult setFieldValue ( string fieldName, const AssetFieldValue& fieldValue ) {
    
        const Schema& schema = this->mLedger->getSchema ();
    
        string assetType = this->mType.get ();
        const AssetDefinition* assetDefinition = schema.getDefinitionOrNull ( assetType );
        if ( !assetDefinition ) return false;

        return this->setFieldValue ( fieldName, *assetDefinition, fieldValue );
    }
};

} // namespace Volition
#endif
