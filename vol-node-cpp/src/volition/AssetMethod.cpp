// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Asset.h>
#include <volition/AssetMethod.h>

namespace Volition {

//================================================================//
// AssetMethod
//================================================================//

//----------------------------------------------------------------//
bool AssetMethod::checkInvocation ( const map < string, shared_ptr < const Asset >>& assetParams, const map < string, AssetFieldValue >& constParams ) const {

    Arguments::const_iterator argIt = this->mAssetArgs.cbegin ();
    for ( ; argIt != this->mAssetArgs.cend (); ++argIt ) {
    
        string paramName = argIt->first;
        const AssetMethodParamDesc& arg = argIt->second;
        shared_ptr < const AbstractSquap > qualifier = arg.mQualifier;
        
        map < string, shared_ptr < const Asset >>::const_iterator assetParamIt = assetParams.find ( paramName );
        if ( assetParamIt == assetParams.end ()) return false;
        if ( !assetParamIt->second ) return false;
        if ( qualifier && !qualifier->evaluate ( SquapEvaluationContext ( *assetParamIt->second ))) return false;
    }

    argIt = this->mConstArgs.cbegin ();
    for ( ; argIt != this->mConstArgs.cend (); ++argIt ) {
    
        string paramName = argIt->first;
        const AssetMethodParamDesc& arg = argIt->second;
        shared_ptr < const AbstractSquap > qualifier = arg.mQualifier;
            
        map < string, AssetFieldValue >::const_iterator constParamIt = constParams.find ( paramName );
        if ( constParamIt == constParams.end ()) return false;
        if ( qualifier && !qualifier->evaluate ( SquapEvaluationContext ( constParamIt->second ))) return false;
    }

    if ( this->mConstraints.size () > 0 ) {
    
        SquapEvaluationContext context ( assetParams, constParams );
    
        Constraints::const_iterator constraintIt = this->mConstraints.cbegin ();
        for ( ; constraintIt != this->mConstraints.cend (); ++constraintIt ) {
            shared_ptr < const AbstractSquap > qualifier = *constraintIt;
            assert ( qualifier );
            if ( !qualifier->evaluate ( context )) return false;
        }
    }
    return true;
}

} // namespace Volition
