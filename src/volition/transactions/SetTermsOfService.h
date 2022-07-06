// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETTERMSOFSERVICE_H
#define VOLITION_TRANSACTIONS_SETTERMSOFSERVICE_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/ContractWithDigest.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetTermsOfService
//================================================================//
class SetTermsOfService :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SET_TERMS_OF_SERVICE" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    string          mText;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "text",      this->mText  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "text",      this->mText  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_TERMS_OF_SERVICE )) return "Permission denied.";
        context.mLedger.setTermsOfService ( ContractWithDigest ( this->mText ));
        return true;
    }
};

} // namespace Transactions
} // namespace Volition
#endif
