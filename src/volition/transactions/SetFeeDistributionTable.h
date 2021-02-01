// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTIONS_SETFEEDISTRIBUTIONTABLE_H
#define VOLITION_TRANSACTIONS_SETFEEDISTRIBUTIONTABLE_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/FeeDistributionTable.h>

namespace Volition {
namespace Transactions {

//================================================================//
// SetFeeDistributionTable
//================================================================//
class SetFeeDistributionTable :
    public AbstractTransactionBody {
public:

    TRANSACTION_TYPE ( "SET_FEE_DISTRIBUTION_TABLE" )
    TRANSACTION_WEIGHT ( 1 )
    TRANSACTION_MATURITY ( 0 )

    FeeDistributionTable    mDistributionTable;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "distributionTable",     this->mDistributionTable  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "distributionTable",     this->mDistributionTable  );
    }

    //----------------------------------------------------------------//
    TransactionResult AbstractTransactionBody_apply ( TransactionContext& context ) const override {
                
        if ( !context.mKeyEntitlements.check ( KeyEntitlements::SET_FEE_DISTRIBUTION_TABLE )) return "Permission denied.";
        return context.mLedger.setFeeDistributionTable ( this->mDistributionTable );
    }
};

} // namespace Transactions
} // namespace Volition
#endif
