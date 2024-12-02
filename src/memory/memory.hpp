#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>  // Para tabelas de páginas e debug

class Memory {
public:
    Memory();
    ~Memory();

    // Operações de leitura/escrita
    uint8_t read8(uint32_t address);
    uint16_t read16(uint32_t address);
    uint32_t read32(uint32_t address);

    void write8(uint32_t address, uint8_t value);
    void write16(uint32_t address, uint16_t value);
    void write32(uint32_t address, uint32_t value);

    void printMemory(uint32_t startAddress, uint32_t size);
    void loadBIOS(const std::string& biosPath);

    // Ferramentas de debug
    void enableDebugLogging(const std::string& logPath);
    void logAccess(uint32_t address, const std::string& operation, uint32_t size);

//protected:
    // Regiões de memória
    std::vector<uint32_t> ram;      // RAM (32MB)
    std::vector<uint32_t> bios;     // BIOS (4MB)
    std::vector<uint8_t> spram;     // Scratchpad RAM (16KB)

    // Cache de Dados L1 (16KB)
    struct CacheLineData {
        uint32_t tag;
        uint32_t data[8];  // Linha de cache com 8 palavras (32 bytes)
        bool valid;
    };
    std::vector<CacheLineData> dataCache;

    // Cache de Instrução L1 (16KB)
    struct CacheLineInst {
        uint32_t tag;
        uint32_t data[8];  // Linha de cache com 8 palavras (32 bytes)
        bool valid;
    };
    std::vector<CacheLineInst> instructionCache;

    // Tabela de páginas
    struct PageTableEntry {
        uint32_t physicalAddress;
        bool valid;
    };

    std::unordered_map<uint32_t, PageTableEntry> pageTable;

    // Debug
    bool debugLoggingEnabled;
    std::ofstream debugLogFile;

    // Constantes de tamanho e endereços
    static const uint32_t RAM_SIZE = 0x02000000;          // 32MB de RAM
    static const uint32_t BIOS_SIZE = 0x00400000;         // 4MB de BIOS
    static const uint32_t SPRAM_SIZE = 0x00004000;        // 16KB de Scratchpad
    static const uint32_t BIOS_START_ADDRESS = 0xBFC00000;
    static const uint32_t SPRAM_START_ADDRESS = 0x70000000;

    // Métodos auxiliares
    bool isValidAddress(uint32_t address, uint32_t size);
    uint32_t translateAddress(uint32_t address);

    // Funções de cache
    uint32_t cacheReadData(uint32_t address);
    uint32_t cacheReadInst(uint32_t address);
    void cacheWriteData(uint32_t address, uint32_t value);
    void cacheWriteInst(uint32_t address, uint32_t value);
};

#endif // MEMORY_HPP