// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_PSUEDORANDOM_H
#define VOLITION_PSUEDORANDOM_H

#include <volition/common.h>
#include <volition/Digest.h>

namespace Volition {

// NOTE: this is for "deterministic" testing support and for ad-hoc random use cases that
// *do not* require cryptographic entropy.

//================================================================//
// Psuedorandom
//================================================================//
class Psuedorandom {
private:

    mt19937             mPRNG;

public:

    //----------------------------------------------------------------//
    Psuedorandom () :
        mPRNG ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    Psuedorandom ( const Digest& digest ) {
        this->seed ( digest );
    }
    
    //----------------------------------------------------------------//
    ~Psuedorandom () {
    }

    //----------------------------------------------------------------//
    double randomDouble () {
        return ( double )this->mPRNG () / ( double )mt19937::max ();
        
    }

    //----------------------------------------------------------------//
    u32 randomInt32 () {
        return this->mPRNG ();
    }
    
    //----------------------------------------------------------------//
    void seed ( const Digest& digest ) {
    
        size_t nSeeds = digest.size () >> 2;
        const u32* seedVals = ( const u32* )digest.data ();

        std::seed_seq sseq {
            seedVals [ 0 % nSeeds ],
            seedVals [ 1 % nSeeds ],
            seedVals [ 2 % nSeeds ],
            seedVals [ 3 % nSeeds ],
            seedVals [ 4 % nSeeds ],
            seedVals [ 5 % nSeeds ],
            seedVals [ 6 % nSeeds ],
            seedVals [ 7 % nSeeds ],
        };
        this->mPRNG.seed ( sseq );
    }
    
    //----------------------------------------------------------------//
    void seed ( string seedStr ) {
    
        Digest digest ( seedStr );
        this->seed ( digest );
    }
};

} // namespace Volition
#endif
