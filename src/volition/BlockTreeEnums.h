// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_BLOCKTREENUMS_H
#define VOLITION_BLOCKTREENUMS_H

namespace Volition {

enum kBlockTreeAppendResult {
    APPEND_OK,
    ALREADY_EXISTS,
    MISSING_PARENT,
    TOO_SOON,
    INCOMPATIBLE,
};

// this represents the health of the branch. it is updated by search and when
// pushing a block.
enum kBlockTreeBranchStatus {
    BRANCH_STATUS_NEW           = 0x01,
    BRANCH_STATUS_COMPLETE      = 0x02,
    BRANCH_STATUS_MISSING       = 0x04,
    BRANCH_STATUS_INVALID       = 0x08,
};

enum kBlockTreeSearchStatus {
    SEARCH_STATUS_NEW,
    SEARCH_STATUS_PROVISIONAL,
    SEARCH_STATUS_HAS_BLOCK,
};

} // namespace Volition
#endif
