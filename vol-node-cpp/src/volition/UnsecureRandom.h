// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_UNSECURERANDOM_H
#define VOLITION_UNSECURERANDOM_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Factory.h>
#include <volition/Singleton.h>

namespace Volition {

// NOTE: this is for "deterministic" testing support and for ad-hoc random use cases that
// *do not* require cryptographic entropy.

//================================================================//
// UnsecureRandom
//================================================================//
class UnsecureRandom :
    public Singleton < UnsecureRandom > {
private:

    mt19937                                             mPRNG;
    uniform_real_distribution < double >                mUniformDistribution;

public:

    //----------------------------------------------------------------//
    double random () {

        return this->mUniformDistribution ( this->mPRNG );
    }

    //----------------------------------------------------------------//
    size_t random ( size_t base, size_t top ) {

        // TODO: this is silly, but good enough for now
        size_t range = ( top - base ) + 1;
        double r = this->random () * ( double )( range );
        return base + (( size_t )floor ( r ) % range );
    }

    //----------------------------------------------------------------//
    void reset () {
    
        this->mPRNG                     = mt19937 ( 0 );
        this->mUniformDistribution      = uniform_real_distribution < double >( 0, 1 );
        
    }

    //----------------------------------------------------------------//
    UnsecureRandom () :
        mPRNG ( 0 ),
        mUniformDistribution ( 0, 1 ) {
    }
    
    //----------------------------------------------------------------//
    ~UnsecureRandom () {
    }
};

} // namespace Volition
#endif
