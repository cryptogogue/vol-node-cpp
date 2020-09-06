// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THECONTEXT_H
#define VOLITION_THECONTEXT_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Digest.h>
#include <volition/Singleton.h>

namespace Volition {

class Block;

//================================================================//
// TheContext
//================================================================//
class TheContext :
    public Singleton < TheContext > {
private:
    
    double          mRewriteWindowInSeconds;

public:

    //----------------------------------------------------------------//
    const Digest&       getGenesisBlockDigest       () const;
    const CryptoKey&    getGenesisBlockKey          () const;
    double              getWindow                   () const;
                        TheContext                  ();
    void                setGenesisBlockDigest       ( const Digest& digest );
    void                setGenesisBlockKey          ( const CryptoKey& key );
    void                setWindow                   ( double window );
};

} // namespace Volition
#endif
