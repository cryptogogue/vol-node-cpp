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

    enum class ScoringMode {
        ALLURE,
        INTEGER,
    };

private:

    CryptoKey       mKey;
    Digest          mDigest;

    ScoringMode     mScoringMode;
    size_t          mScoringModulo;
    
    u64             mWindow;

public:

    //----------------------------------------------------------------//
    int                 compare                     ( const Block& block0, const Block& block1 );
    string              getCurve                    () const;
    const Digest&       getGenesisBlockDigest       () const;
    const CryptoKey&    getGenesisBlockKey          () const;
    ScoringMode         getScoringMode              () const;
    size_t              getScoringModulo            () const;
    u64                 getWindow                   () const;
                        TheContext                  ();
    void                setGenesisBlockDigest       ( const Digest& digest );
    void                setGenesisBlockKey          ( const CryptoKey& key );
    void                setScoringMode              ( ScoringMode scoringMode, size_t modulo = 0 );
    void                setWindow                   ( u64 window );
};

} // namespace Volition
#endif
