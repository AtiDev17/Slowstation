#include "logger/Logger.hpp"
#include "bus/Bios.hpp"
#include "bus/Ram.hpp"
#include "bus/Interconnect.hpp"
#include "cpu/Cpu.hpp"

using namespace slowstation::bus;
using namespace slowstation::cpu;

/**
 * @brief Entry point for the SlowStation PS1 Emulator.
 *
 * Initializes the system components (BIOS, RAM, Interconnect, CPU)
 * and starts the execution loop.
 */
int main(int argc, char** argv)
{
    Logger logger;
    logger.Info("SlowStation: PSX Emulator starting...");

    try
    {
        // 1. Initialize the 512KB BIOS ROM.
        auto bios = std::make_unique<Bios>(R"(C:\Users\YS\CLionProjects\Slowstation\SCPH1001.BIN)");
        
        // 2. Initialize the 2MB Main System RAM.
        auto ram = std::make_unique<Ram>();

        // 3. Connect all devices to the Interconnect (The System Bus).
        auto interconnect = std::make_unique<Interconnect>(std::move(bios), std::move(ram));
        
        // 4. Initialize the MIPS R3000A CPU with the Interconnect.
        Cpu cpu(std::move(interconnect), logger);

        // 5. Trigger the Power-On Reset sequence.
        cpu.reset();

        // 6. Execution Loop: Run for a set number of cycles to observe BIOS behavior.
        // During the BIOS boot, we expect to see memory clearing and system initialization.
        for (int i = 0; i < 1000; ++i)
        {
            cpu.step();
        }
    }
    catch (const std::exception& e)
    {
        logger.Error(std::format("Fatal Error: {}", e.what()));
        return 1;
    }
    
    return 0;
}
