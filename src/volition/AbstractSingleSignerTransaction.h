// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSINGLESIGNERTRANSACTION_H
#define VOLITION_ABSTRACTSINGLESIGNERTRANSACTION_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>

namespace Volition {

//================================================================//
// AbstractSingleSignerTransaction
//================================================================//
class AbstractSingleSignerTransaction :
    public AbstractTransaction {
protected:

    SerializableUniquePtr < TransactionMakerSignature >     mMakerSignature;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
    
        serializer.serialize ( "maker",     this->mMakerSignature );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
    
        serializer.serialize ( "maker",     this->mMakerSignature );
    }
    
    //----------------------------------------------------------------//
    bool AbstractTransaction_checkSignature ( Ledger& ledger ) const override {

        const TransactionMakerSignature* makerSignature = this->mMakerSignature.get ();
        return ( makerSignature && ledger.checkMakerSignature ( makerSignature ));
    }
    
    //----------------------------------------------------------------//
    void AbstractTransaction_incrementNonce ( Ledger& ledger ) const override {

        ledger.incrementNonce ( this->mMakerSignature.get ());
    }
};

} // namespace Volition
#endif
