// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_REGISTERMINER_H
#define VOLITION_TRANSACTION_REGISTERMINER_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>

namespace Volition {
namespace Transaction {

//================================================================//
// RegisterMiner
//================================================================//
class RegisterMiner :
    public AbstractTransaction {
private:

    string                                  mMinerID;
    string                                  mURL;    
    string                                  mKeyName;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;
    void                    AbstractTransaction_apply           ( State& state ) const override;

public:

    TRANSACTION_TYPE ( "REGISTER_MINER" )
    TRANSACTION_WEIGHT ( 1 )

    //----------------------------------------------------------------//
                            RegisterMiner           ();
                            RegisterMiner           ( string minerID, string url, string keyName );
                            ~RegisterMiner          ();
};

} // namespace Transaction
} // namespace Volition
#endif
