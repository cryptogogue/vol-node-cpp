// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMULATION_ABSTRACTSCENARIO_H
#define VOLITION_SIMULATION_ABSTRACTSCENARIO_H

#include <volition/common.h>
#include <volition/Block.h>
#include <volition/BlockTree.h>
#include <volition/Chain.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/simulation/Analysis.h>
#include <volition/simulation/SimMiningMessenger.h>
#include <volition/simulation/Simulator.h>
#include <volition/Transaction.h>

namespace Volition {
namespace Simulation {

#define SCENARIO_BASE_PORT(port)                                \
    size_t AbstractScenario_getBasePort () const override {     \
        return port;                                            \
    }

#define SCENARIO_REPORT_MODE(reportMode)                        \
    Simulator::ReportMode AbstractScenario_getReportMode () const override { \
        return reportMode;                                      \
    }

#define SCENARIO_SIZE(size)                                     \
    size_t AbstractScenario_getSize () const override {         \
        return size;                                            \
    }

//================================================================//
// AbstractScenario
//================================================================//
class AbstractScenario {
protected:

    friend class Simulator;

    //----------------------------------------------------------------//
    virtual void AbstractScenario_control ( Simulator& simulator, size_t step ) const {
        UNUSED ( simulator );
        UNUSED ( step );
    }
    
    //----------------------------------------------------------------//
    virtual size_t AbstractScenario_getBasePort () const {
    
        return 9090;
    }
    
    //----------------------------------------------------------------//
    virtual Simulator::ReportMode AbstractScenario_getReportMode () const {
    
        return Simulator::REPORT_SUMMARY;
    }
    
    //----------------------------------------------------------------//
    virtual size_t AbstractScenario_getSize () const {
    
        return 16;
    }
    
    //----------------------------------------------------------------//
    virtual void AbstractScenario_setup ( Simulator& simulator ) const {
        UNUSED ( simulator );
    }

public:

    //----------------------------------------------------------------//
    AbstractScenario () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractScenario () {
    }
};

} // namespace Simulation
} // namespace Volition
#endif
