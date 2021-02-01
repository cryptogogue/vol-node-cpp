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
class Schema;

//================================================================//
// TheContext
//================================================================//
class TheContext :
    public Singleton < TheContext > {
public:

    enum class ScoringMode {
        ALLURE,
        INTEGER,
    };

private:

    friend class Ledger;

    ScoringMode     mScoringMode;
    size_t          mScoringModulo;
    
    double          mRewriteWindowInSeconds;

    // TODO: park this here for now
    map < string, Schema > mSchemaCache;

public:

    //----------------------------------------------------------------//
    ScoringMode                     getScoringMode              () const;
    size_t                          getScoringModulo            () const;
    double                          getWindow                   () const;
                                    TheContext                  ();
    void                            setGenesisBlockDigest       ( const Digest& digest );
    void                            setGenesisBlockKey          ( const CryptoKey& key );
    void                            setScoringMode              ( ScoringMode scoringMode, size_t modulo = 0 );
    void                            setWindow                   ( double window );
};

} // namespace Volition
#endif
