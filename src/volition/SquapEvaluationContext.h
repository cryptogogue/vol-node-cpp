// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQUAPEVALUATIONCONTEXT_H
#define VOLITION_SQUAPEVALUATIONCONTEXT_H

#include <volition/common.h>
#include <volition/Asset.h>
#include <volition/AssetFieldValue.h>
#include <volition/FNV1a.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

// Schema QUAlifier oPerator

//================================================================//
// SquapEvaluationContext
//================================================================//
class SquapEvaluationContext {
private:

    map < string, shared_ptr < const Asset >>   mAssetParams;
    map < string, AssetFieldValue >             mValueParams;

public:

    //----------------------------------------------------------------//
    const Asset* getAsset ( string paramName ) const {
        
        map < string, shared_ptr < const Asset >>::const_iterator paramIt = this->mAssetParams.find ( paramName );
        if ( paramIt != this->mAssetParams.cend ()) {
            return paramIt->second.get ();
        }
        return NULL;
    };

    //----------------------------------------------------------------//
    AssetFieldValue getValue ( string paramName ) const {
        
        map < string, AssetFieldValue >::const_iterator paramIt = this->mValueParams.find ( paramName );
        if ( paramIt != this->mValueParams.cend ()) {
            return paramIt->second;
        }
        return AssetFieldValue ();
    };

    //----------------------------------------------------------------//
    SquapEvaluationContext ( shared_ptr < const Asset > assetParam ) {
    
        this->mAssetParams [ "" ] = assetParam;
    }
    
    //----------------------------------------------------------------//
    SquapEvaluationContext ( const AssetFieldValue& value ) {
    
        this->mValueParams [ "" ] = value;
    }
    
    //----------------------------------------------------------------//
    SquapEvaluationContext ( const map < string, shared_ptr < const Asset >>& assetParams, const map < string, AssetFieldValue >& valueParams ) :
        mAssetParams ( assetParams ),
        mValueParams ( valueParams ) {
    }
};

} // namespace Volition
#endif
