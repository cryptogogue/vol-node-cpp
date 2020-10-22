// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTCONTROLCOMMANDBODY_H
#define VOLITION_ABSTRACTCONTROLCOMMANDBODY_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Digest.h>
#include <volition/LedgerResult.h>
#include <volition/Miner.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

#define COMMAND_CONTROL_LEVEL(level)                                    \
    Miner::Control AbstractControlCommandBody_controlLevel () const override { \
        return level;                                                   \
    }

#define COMMAND_TYPE(typeString)                                        \
    static constexpr const char* TYPE_STRING = typeString;              \
    string AbstractControlCommandBody_typeString () const override {    \
        return TYPE_STRING;                                             \
    }

class Miner;
class ControlCommand;

//================================================================//
// AbstractControlCommandBody
//================================================================//
class AbstractControlCommandBody :
    public AbstractSerializable {
protected:

    friend class ControlCommand;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

    //----------------------------------------------------------------//
    virtual Miner::Control      AbstractControlCommandBody_controlLevel     () const;
    virtual LedgerResult        AbstractControlCommandBody_execute          ( Miner& miner ) const;
    virtual string              AbstractControlCommandBody_typeString       () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractControlCommandBody              ();
                            ~AbstractControlCommandBody             ();
    LedgerResult            control                                 ( Miner& miner ) const;
    Miner::Control          controlLevel                            () const;
    string                  typeString                              () const;
};

} // namespace Volition
#endif
