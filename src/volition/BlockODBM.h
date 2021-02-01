// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKODBM_H
#define VOLITION_BLOCKODBM_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/LedgerObjectFieldODBM.h>

namespace Volition {

//================================================================//
// BlockODBM
//================================================================//
class BlockODBM {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_block ( u64 height ) {
        return LedgerKey ([ = ]() { return Format::write ( "block.%d", height ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_header ( u64 height ) {
        return LedgerKey ([ = ]() { return Format::write ( "block.%d.header", height ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_blockHash ( u64 height ) {
        return LedgerKey ([ = ]() { return Format::write ( "block.%d.hash", height ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_blockPose ( u64 height ) {
        return LedgerKey ([ = ]() { return Format::write ( "block.%d.pose", height ); });
    }

public:

    ConstOpt < Ledger >                     mLedger;
    u64                                     mHeight;

    LedgerFieldODBM < string >              mHash;
    LedgerFieldODBM < string >              mPose;

    LedgerObjectFieldODBM < Block >         mBlock;
    LedgerObjectFieldODBM < BlockHeader >   mHeader;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mHash.exists ();
    }

    //----------------------------------------------------------------//
    BlockODBM ( ConstOpt < Ledger > ledger, u64 height ) :
        mLedger ( ledger ),
        mHeight ( height ) {
        
        this->mHash         = LedgerFieldODBM < string >( this->mLedger,                keyFor_blockHash ( this->mHeight ),             "" );
        this->mPose         = LedgerFieldODBM < string >( this->mLedger,                keyFor_blockPose ( this->mHeight ),             "" );
        this->mBlock        = LedgerObjectFieldODBM < Block >( this->mLedger,           keyFor_block ( this->mHeight ));
        this->mHeader       = LedgerObjectFieldODBM < BlockHeader >( this->mLedger,     keyFor_header ( this->mHeight ));
    }
};

} // namespace Volition
#endif
