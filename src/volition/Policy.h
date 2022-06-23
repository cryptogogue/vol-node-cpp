// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_POLICY_H
#define VOLITION_POLICY_H

#include <volition/common.h>
#include <volition/Entitlements.h>
#include <volition/serialization/Serialization.h>

// Keys and accounts have "policies," which limit what they can do. The "base" policy
// is awlays the most permissive; all other policies must be more restrictive.

// No key or account can create another key or account with a less restrictive policy. In
// addition, the type of policies a key or account can create may be restricted by a "bequest."
// If a is specified, then any derived policy must be no less restrictive than the bequest.

// Policy entitlements are derived from "base" restrictions, which are held in the ledger.
// To the "base" restrictions, further restrictions may be applied. This way, a set of "global"
// entitlement profiles can be created and shared.

namespace Volition {

class AbstractLedger;

//================================================================//
// Policy
//================================================================//
class Policy :
    public AbstractSerializable {
private:

    string                                          mBase; // If empty string, will load the base (i.e. most permissive) policy.
    SerializableSharedConstPtr < Entitlements >     mRestrictions;

    //----------------------------------------------------------------//
    void        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    shared_ptr < Entitlements >     applyRestrictions       ( const Entitlements& entitlements ) const;
    string                          getBase                 () const;
    const Entitlements*             getRestrictions         () const;
    bool                            isMatchOrSubsetOf       ( const Entitlements& entitlements ) const;
    bool                            isValid                 ( const AbstractLedger& ledger ) const;
                                    Policy                  ();
                                    ~Policy                 ();
    void                            setRestrictions         ( shared_ptr < const Entitlements > restrictions );
};

} // namespace Volition
#endif
