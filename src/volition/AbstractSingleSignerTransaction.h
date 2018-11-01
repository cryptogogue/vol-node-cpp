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
};

} // namespace Volition
#endif
