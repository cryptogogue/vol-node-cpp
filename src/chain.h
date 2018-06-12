// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAIN_H
#define VOLITION_CHAIN_H

#include <Cycle.h>

namespace Volition {

//================================================================//
// Chain
//================================================================//
class Chain :
    public AbstractSerializable {
private:

    vector < unique_ptr < Cycle >>  mCycles;

    //----------------------------------------------------------------//
    bool                    canEdit             ( size_t cycleID, string minerID = "" ) const;
    bool                    canEdit             ( size_t cycleID, const Chain& chain ) const;
    static const Chain*     choose              ( size_t cycleID, const Chain& prefer, const Chain& other );
    size_t                  findMax             ( size_t cycleID ) const;
    Cycle*                  getTopCycle         ();
    const Cycle*            getTopCycle         () const;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

public:

    //----------------------------------------------------------------//
    void                    apply               ( State& state ) const;
                            Chain               ();
                            ~Chain              ();
    static const Chain*     choose              ( const Chain& chain0, const Chain& chain1 );
    Cycle*                  nextCycle           ( string minerID, bool force );
    void                    print               ( const char* pre = 0, const char* post = "\n" ) const;
};

} // namespace Volition
#endif

