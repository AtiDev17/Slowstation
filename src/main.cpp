#include "logger/Logger.hpp"
#include "bus/Bios.hpp"
#include "bus/Interconnect.hpp"
#include "cpu/Cpu.hpp"

int main(int argc, char** argv)
{
    Logger logger;
    logger.Info("SlowStation: PSX Emulator starting...");

    auto bios = std::make_unique<slowstation::bus::Bios>(R"(C:\Users\YS\CLionProjects\Slowstation\SCPH1001.BIN)");
    auto interconnect = std::make_unique<slowstation::bus::Interconnect>(std::move(bios));
    slowstation::cpu::Cpu cpu(std::move(interconnect), logger);

    cpu.reset();

    for (int i = 0; i < 10; ++i)
    {
        cpu.step();
    }
    return 0;
}