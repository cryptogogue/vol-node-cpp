// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONSTATUS_H
#define VOLITION_TRANSACTIONSTATUS_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class TransactionEnvelope;

//================================================================//
// TransactionStatus
//================================================================//
class TransactionStatus :
    public AbstractSerializable {
public:

    enum Code {
        UNKNOWN,        // no record of transaction in ledger or queue
        PENDING,        // in queue
        ACCEPTED,       // in ledger
        REJECTED,       // in queue, with error
        IGNORED,        // in queue, ignored (no gratuity, etc.)
    };

    Code        mCode;
    string      mMessage;
    string      mUUID;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        UNUSED ( serializer );
        assert ( false );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        serializer.serialize ( "code",      this->getStatusCodeString ());
        serializer.serialize ( "message",   this->mMessage );
        serializer.serialize ( "uuid",      this->mUUID );
    }
    
    //----------------------------------------------------------------//
    string getStatusCodeString () const {
    
        switch ( this->mCode ) {
            case PENDING:       return "PENDING";
            case ACCEPTED:      return "ACCEPTED";
            case REJECTED:      return "REJECTED";
            case IGNORED:       return "IGNORED";
            case UNKNOWN:
            default:
                break;
        }
        return "UNKNOWN";
    }
    
    //----------------------------------------------------------------//
    TransactionStatus () :
        mCode ( UNKNOWN ) {
    }
    
    //----------------------------------------------------------------//
    TransactionStatus ( Code code, string message, string uuid = "" ) :
        mCode ( code ),
        mMessage ( message ),
        mUUID ( uuid ) {
    }
};

} // namespace Volition
#endif
