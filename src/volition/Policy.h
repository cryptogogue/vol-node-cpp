// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_POLICY_H
#define VOLITION_POLICY_H

#include <volition/common.h>

#include <volition/AbstractHashable.h>
#include <volition/AbstractSerializable.h>

namespace Volition {

//================================================================//
// Policy
//================================================================//
class Policy :
    public AbstractHashable,
    public AbstractSerializable {
private:

    //----------------------------------------------------------------//
    void                AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

//    static constexpr const char* KEY_FINAL                    = "KEY_FINAL";
//    static constexpr const char* KEY_TRANSFER_LIMIT           = "KEY_TRANSFER_LIMIT";
//
//    static constexpr const char* ACCOUNT_LIMIT                = "ACCOUNT_LIMIT";
//    static constexpr const char* ACCOUNT_OVERFLOW             = "ACCOUNT_OVERFLOW";

    //----------------------------------------------------------------//
                        Policy                              ();
                        ~Policy                             ();
};

} // namespace Volition
#endif
