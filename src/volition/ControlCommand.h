// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CONTROLCOMMAND_H
#define VOLITION_CONTROLCOMMAND_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/serialization/Serialization.h>
#include <volition/Signature.h>
#include <volition/TheControlCommandBodyFactory.h>

namespace Volition {

class Ledger;
class Miner;

//================================================================//
// ControlCommand
//================================================================//
class ControlCommand :
    public AbstractSerializable {
protected:

    typedef SerializableSharedPtr < AbstractControlCommandBody, ControlCommandBodyFactory > CommandBodyPtr;
    typedef SerializableSharedPtr < Signature > SignaturePtr;

    CommandBodyPtr          mBody;          // serialized from/to *body*
    string                  mBodyString;    // store this verbatim
    SignaturePtr            mSignature;     // signatures for *body*
    
    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
                            ControlCommand          ();
                            ~ControlCommand         ();
    Miner::Control          controlLevel            () const;
    LedgerResult            execute                 ( Miner& miner ) const;
    void                    setBody                 ( shared_ptr < AbstractControlCommandBody > body );
    string                  typeString              () const;
};

} // namespace Volition
#endif
