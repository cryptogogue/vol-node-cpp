// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCOPEDPROFILE_H
#define VOLITION_SCOPEDPROFILE_H

#include <volition/common.h>
#include <volition/Format.h>

namespace Volition {

//================================================================//
// ScopedProfile
//================================================================//
class ScopedProfile {
private:

    string                                      mLabel;
    chrono::high_resolution_clock::time_point   mT0;

public:

    //----------------------------------------------------------------//
    ScopedProfile ( string label ) :
        mLabel ( label ) {
        this->mT0 = chrono::high_resolution_clock::now ();
    }
    
    //----------------------------------------------------------------//
    ~ScopedProfile () {
    
        chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now ();
        chrono::milliseconds span = chrono::duration_cast < chrono::milliseconds >( t1 - this->mT0 );
        
        LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, "PROFILE %llums: %s", span.count (), this->mLabel.c_str ());
    }
};

} // namespace Volition
#endif
