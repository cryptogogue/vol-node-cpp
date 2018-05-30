#ifndef VLBLOCK_H
#define VLBLOCK_H

#include "common.h"

//================================================================//
// VLBlock
//================================================================//
class VLBlock {
private:

    friend class Context;

    size_t      mBlockID;
    string      mBlockHash;
    string      mPrevBlockHash;

public:

    //----------------------------------------------------------------//
    void        Sign                ();
                VLBlock             ();
                ~VLBlock            ();
};

#endif
