// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGERRESULT_H
#define VOLITION_LEDGERRESULT_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// LedgerResult
//================================================================//
class LedgerResult {
private:

    bool        mStatus;
    string      mMessage;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mStatus );
    }

    //----------------------------------------------------------------//
    string getMessage () const {
        return this->mMessage;
    }
    //----------------------------------------------------------------//
    LedgerResult ( bool status ) :
        mStatus ( status ),
        mMessage ( status ? "OK" : "UNSPECIFIED ERROR" ) {
    }

    //----------------------------------------------------------------//
    LedgerResult ( const char* message ) :
        mMessage ( message ) {
        this->mStatus = ( this->mMessage.size () == 0 );
    }

    //----------------------------------------------------------------//
    LedgerResult ( string message ) :
        mStatus ( message.size () == 0 ),
        mMessage ( message ) {
    }
    
    //----------------------------------------------------------------//
    LedgerResult ( const LedgerResult& other ) {
        *this = other;
    }
};

} // namespace Volition
#endif
