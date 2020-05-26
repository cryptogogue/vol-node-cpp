// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Asset.h>
#include <volition/AssetMethod.h>

namespace Volition {

//================================================================//
// AssetMethod
//================================================================//

//----------------------------------------------------------------//
bool AssetMethod::checkInvocation ( const map < string, shared_ptr < const Asset >>& params ) const {

    map < string, shared_ptr < const Asset >>::const_iterator paramIt = params.cbegin ();
    for ( ; paramIt != params.cend (); ++paramIt ) {
    
        string paramName                    = paramIt->first;
        shared_ptr < const Asset > asset    = paramIt->second;
        if ( !( asset && this->qualifyAssetArg ( paramName, *asset ))) return false;
    }

    if ( this->mConstraints.size () > 0 ) {
    
        SquapEvaluationContext context ( params );
    
        Constraints::const_iterator constraintIt = this->mConstraints.cbegin ();
        for ( ; constraintIt != this->mConstraints.cend (); ++constraintIt ) {
            shared_ptr < const AbstractSquap > qualifier = *constraintIt;
            assert ( qualifier );
            if ( !qualifier->evaluate ( context )) return false;
        }
    }
    return true;
}

//----------------------------------------------------------------//
bool AssetMethod::qualifyAssetArg ( string argName, const Asset& asset ) const {

    Qualifiers::const_iterator qualifierIt = this->mAssetArgs.find ( argName );
    if ( qualifierIt != this->mAssetArgs.cend ()) {
        shared_ptr < const AbstractSquap > qualifier = qualifierIt->second;
        return qualifier->evaluate ( SquapEvaluationContext ( asset ));
    }
    return true; // no qualifier; always eval to 'true'
}

//----------------------------------------------------------------//
bool AssetMethod::qualifyConstArg ( string argName, const AssetFieldValue& value ) const {
    UNUSED ( argName );
    UNUSED ( value );

    return false;
}

} // namespace Volition
