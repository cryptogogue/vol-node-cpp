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

public:

    //----------------------------------------------------------------//
    int                 compare                     ( const Block& block0, const Block& block1 );
    string              getCurve                    () const;
    const Digest&       getGenesisBlockDigest       () const;
    const CryptoKey&    getGenesisBlockKey          () const;
    ScoringMode         getScoringMode              () const;
                        TheContext                  ();
    void                setGenesisBlockDigest       ( const Digest& digest );
    void                setGenesisBlockKey          ( const CryptoKey& key );
    void                setScoringMode              ( ScoringMode scoringMode );
};

} // namespace Volition
#endif
