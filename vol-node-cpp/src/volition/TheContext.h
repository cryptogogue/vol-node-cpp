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
public:

    enum RewriteMode {
        REWRITE_NONE,
        REWRITE_WINDOW,
        REWRITE_ANY,
    };

private:
    
    RewriteMode         mRewriteMode;
    double              mRewriteWindowInSeconds;

public:

    //----------------------------------------------------------------//
    RewriteMode         getRewriteMode              () const;
    double              getWindow                   () const;
                        TheContext                  ();
    void                setGenesisBlockDigest       ( const Digest& digest );
    void                setGenesisBlockKey          ( const CryptoKey& key );
    void                setRewriteMode              ( RewriteMode mode );
    void                setRewriteWindow            ( double window );
};

} // namespace Volition
#endif
