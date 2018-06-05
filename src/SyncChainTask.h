#ifndef VOLITION_SYNCCHAINTASK_H
#define VOLITION_SYNCCHAINTASK_H

#include "common.h"
#include "MinerInfo.h"
#include "Singleton.h"

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//
class SyncChainTask :
    public Poco::Task {
private:

    //----------------------------------------------------------------//
    void        runTask             () override;

public:

    //----------------------------------------------------------------//
                SyncChainTask       ();
                ~SyncChainTask      ();
};

} // namespace Volition
#endif
