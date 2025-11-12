#include "emc_patcher.h"
#include "file_utils.h"
#include "board.h"


#define MC_BASE 0x70019000
#define EMC_BASE 0x7001B000
#define MC_EMC_BASE_SIZE 0x1000
#define EMC_TIMING_CONTROL_0 0x28
#define EMC_RAS_0 0x34
#define EMC_RAS_BIT_END 5

#define HOSSVC_HAS_MM (hosversionAtLeast(10,0,0))


EMCpatcher* EMCpatcher::instance = nullptr;

EMCpatcher* EMCpatcher::GetInstance()
{
    return instance;
}

void EMCpatcher::Initialize()
{
    if (!instance)
    {
        instance = new EMCpatcher();
        FileUtils::LogLine("[emc] Initialized EMCpatcher");
    }
}

Config *EMCpatcher::GetConfig()
{
    return this->config;
}

void EMCpatcher::Exit()
{
    if (instance)
    {
        FileUtils::LogLine("[emc] Exiting EMCpatcher");
        delete instance;
        instance = nullptr;
    }
}

EMCpatcher::EMCpatcher()
{
    this->config = Config::CreateDefault();
}



EMCpatcher::~EMCpatcher()
{
    delete this->config;
}

void EMCpatcher::Run()
{
    std::scoped_lock lock{this->patcherMutex};
    this->config->Refresh();
    this->ApplyEMCPatch();
}


void EMCpatcher::ApplyEMCPatch()
{
    if(HOSSVC_HAS_MM) { // only for 10.0.0+, older versions need rewrites
        u64 mc_virt_addr = 0;
        u64 mc_out_size = 0;
        u64 emc_virt_addr = 0;
        u64 emc_out_size = 0;
        Result rc;

        // rc = svcQueryMemoryMapping(&mc_virt_addr, &mc_out_size, MC_BASE, MC_EMC_BASE_SIZE); // map mc
        // ASSERT_RESULT_OK(rc, "svcQueryMemoryMapping");
        
        // rc = svcQueryMemoryMapping(&emc_virt_addr, &emc_out_size, EMC_BASE, MC_EMC_BASE_SIZE); // map emc
        // ASSERT_RESULT_OK(rc, "svcQueryMemoryMapping");

        write_reg64(EMC_BASE, EMC_RAS_0, 1);

        write_reg64(EMC_BASE, EMC_TIMING_CONTROL_0, 0x1); // apply shadow regs


        // svcUnmapMemory((void *)mc_virt_addr, (void *)MC_BASE, MC_EMC_BASE_SIZE); // clean up
        // svcUnmapMemory((void *)emc_virt_addr, (void *)EMC_BASE, MC_EMC_BASE_SIZE);

    }
}
