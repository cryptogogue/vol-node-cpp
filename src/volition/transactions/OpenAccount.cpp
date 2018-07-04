// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Hash.h>
#include <volition/Serialize.h>
#include <volition/transactions/OpenAccount.h>

namespace Volition {
namespace Transaction {

//================================================================//
// OpenAccount
//================================================================//

//----------------------------------------------------------------//
OpenAccount::OpenAccount () {
}

//----------------------------------------------------------------//
OpenAccount::OpenAccount ( string accountName, string keyName, const Poco::Crypto::ECKey& key, u64 amount ) :
    mAccountName ( accountName ),
    mKeyName ( keyName ),
    mKey ( make_unique < Poco::Crypto::ECKey >( key )),
    mAmount ( amount ) {
}

//----------------------------------------------------------------//
OpenAccount::~OpenAccount () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void OpenAccount::AbstractHashable_hash ( Poco::DigestOutputStream& digestStream ) const {

    digestStream << this->mAccountName;
    digestStream << this->mKeyName;
    digestStream << this->mAmount;
    
    Hash::hashOrNull ( digestStream, this->mKey.get ());
}

//----------------------------------------------------------------//
void OpenAccount::AbstractSerializable_fromJSON ( const Poco::JSON::Object& object ) {
    AbstractTransaction::AbstractSerializable_fromJSON ( object );
    
    this->mAccountName              = object.optValue < string >( "accountName", "" );
    this->mKeyName                  = object.optValue < string >( "keyName", "" );
    this->mAmount                   = Serialize::getU64FromJSON ( object, "amount", 0 );
    
    this->mKey                      = Serialize::getSerializableFromJSON < Poco::Crypto::ECKey >( object, "key" );
}

//----------------------------------------------------------------//
void OpenAccount::AbstractSerializable_toJSON ( Poco::JSON::Object& object ) const {
    AbstractTransaction::AbstractSerializable_toJSON ( object );
    
    object.set ( "accountName",         this->mAccountName.c_str ());
    object.set ( "keyName",             this->mKeyName.c_str ());
    
    Serialize::setU64ToJSON ( object, "amount", this->mAmount );
    
    Serialize::setSerializableToJSON ( object, "key", this->mKey.get ());
}

//----------------------------------------------------------------//
void OpenAccount::AbstractTransaction_apply ( State& state ) const {
}

} // namespace Transaction
} // namespace Volition
