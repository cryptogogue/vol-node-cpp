// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ENTITLEMENTS_H
#define VOLITION_ENTITLEMENTS_H

#include <volition/common.h>
#include <volition/PathEntitlement.h>

namespace Volition {

//================================================================//
// Entitlements
//================================================================//
class Entitlements :
    public PathEntitlement {
public:

    //----------------------------------------------------------------//
    void apply ( PathEntitlement& other ) const {
    
        Children::const_iterator childIt = this->mChildren.cbegin ();
        for ( ; childIt != this->mChildren.end (); ++childIt ) {
            if ( childIt->second ) {
                other.mChildren [ childIt->first ] = childIt->second;
            }
        }
    }
    
    //----------------------------------------------------------------//
    Entitlements () {
    }
    
    //----------------------------------------------------------------//
    ~Entitlements () {
    }
};

} // namespace Volition
#endif
