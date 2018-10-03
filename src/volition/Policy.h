// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_POLICY_H
#define VOLITION_POLICY_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Policy
//================================================================//
class Policy :
    public AbstractSerializable {
private:

    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

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
