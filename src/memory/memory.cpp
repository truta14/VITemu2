#include "memory.hpp"

Memory::Memory() : debugLoggingEnabled(false) {
    ram.resize(RAM_SIZE / sizeof(uint32_t), 0);
    bios.resize(BIOS_SIZE / sizeof(uint32_t), 0);
    spram.resize(SPRAM_SIZE, 0);

    // Inicializa cache (128 linhas, 32 bytes por linha)
    dataCache.resize(128, {0, {0}, false});  // Cache de Dados
    instructionCache.resize(128, {0, {0}, false});  // Cache de Instruções
}

Memory::~Memory() {
    if (debugLoggingEnabled && debugLogFile.is_open()) {
        debugLogFile.close();
    }
}

uint8_t Memory::read8(uint32_t address) {
    uint32_t word = read32(address & ~0x3); // Alinha ao início da palavra
    return (word >> ((address & 0x3) * 8)) & 0xFF;
}

uint16_t Memory::read16(uint32_t address) {
    uint32_t word = read32(address & ~0x3);
    return (word >> ((address & 0x2) * 8)) & 0xFFFF;
}

uint32_t Memory::read32(uint32_t address) {
    uint32_t physicalAddress = translateAddress(address);
    if (physicalAddress < RAM_SIZE) {
        return ram[physicalAddress / sizeof(uint32_t)];
    } else if (physicalAddress >= BIOS_START_ADDRESS && physicalAddress < BIOS_START_ADDRESS + BIOS_SIZE) {
        return bios[(physicalAddress - BIOS_START_ADDRESS) / sizeof(uint32_t)];
    } else if (physicalAddress >= SPRAM_START_ADDRESS && physicalAddress < SPRAM_START_ADDRESS + SPRAM_SIZE) {
        uint32_t offset = physicalAddress - SPRAM_START_ADDRESS;
        return *(reinterpret_cast<uint32_t*>(&spram[offset]));
    } else {
        logAccess(address, "read", 4);
        return 0;
    }
}

void Memory::write8(uint32_t address, uint8_t value) {
    uint32_t word = read32(address & ~0x3);
    uint32_t shift = (address & 0x3) * 8;
    word = (word & ~(0xFF << shift)) | (value << shift);
    write32(address & ~0x3, word);
}

void Memory::write16(uint32_t address, uint16_t value) {
    uint32_t word = read32(address & ~0x3);
    uint32_t shift = (address & 0x2) * 8;
    word = (word & ~(0xFFFF << shift)) | (value << shift);
    write32(address & ~0x3, word);
}

void Memory::write32(uint32_t address, uint32_t value) {
    uint32_t physicalAddress = translateAddress(address);
    if (physicalAddress < RAM_SIZE) {
        ram[physicalAddress / sizeof(uint32_t)] = value;
    } else if (physicalAddress >= SPRAM_START_ADDRESS && physicalAddress < SPRAM_START_ADDRESS + SPRAM_SIZE) {
        uint32_t offset = physicalAddress - SPRAM_START_ADDRESS;
        *(reinterpret_cast<uint32_t*>(&spram[offset])) = value;
    } else {
        logAccess(address, "write", 4);
    }
}

uint32_t Memory::cacheReadData(uint32_t address) {
    uint32_t physicalAddress = translateAddress(address);
    uint32_t index = (physicalAddress / 32) % dataCache.size();
    uint32_t tag = physicalAddress / (dataCache.size() * 32);

    if (dataCache[index].valid && dataCache[index].tag == tag) {
        return dataCache[index].data[(physicalAddress / 4) % 8];
    }

    // Cache miss: carrega linha de dados
    dataCache[index].tag = tag;
    dataCache[index].valid = true;
    uint32_t baseAddr = physicalAddress & ~0x1F;
    for (int i = 0; i < 8; ++i) {
        dataCache[index].data[i] = read32(baseAddr + i * 4);
    }
    return dataCache[index].data[(physicalAddress / 4) % 8];
}

uint32_t Memory::cacheReadInst(uint32_t address) {
    uint32_t physicalAddress = translateAddress(address);
    uint32_t index = (physicalAddress / 32) % instructionCache.size();
    uint32_t tag = physicalAddress / (instructionCache.size() * 32);

    if (instructionCache[index].valid && instructionCache[index].tag == tag) {
        return instructionCache[index].data[(physicalAddress / 4) % 8];
    }

    // Cache miss: carrega linha de instrução
    instructionCache[index].tag = tag;
    instructionCache[index].valid = true;
    uint32_t baseAddr = physicalAddress & ~0x1F;
    for (int i = 0; i < 8; ++i) {
        instructionCache[index].data[i] = read32(baseAddr + i * 4);
    }
    return instructionCache[index].data[(physicalAddress / 4) % 8];
}

void Memory::cacheWriteData(uint32_t address, uint32_t value) {
    uint32_t physicalAddress = translateAddress(address);
    uint32_t index = (physicalAddress / 32) % dataCache.size();
    uint32_t tag = physicalAddress / (dataCache.size() * 32);
    dataCache[index].tag = tag;
    dataCache[index].data[(physicalAddress / 4) % 8] = value;
    dataCache[index].valid = true;
}

void Memory::cacheWriteInst(uint32_t address, uint32_t value) {
    uint32_t physicalAddress = translateAddress(address);
    uint32_t index = (physicalAddress / 32) % instructionCache.size();
    uint32_t tag = physicalAddress / (instructionCache.size() * 32);
    instructionCache[index].tag = tag;
    instructionCache[index].data[(physicalAddress / 4) % 8] = value;
    instructionCache[index].valid = true;
}

// Tabela de páginas - Tradução de endereços
uint32_t Memory::translateAddress(uint32_t address) {
    uint32_t pageNumber = address >> 12;  // Número da página (12 bits superiores)
    auto entry = pageTable.find(pageNumber);
    if (entry != pageTable.end() && entry->second.valid) {
        return entry->second.physicalAddress + (address & 0xFFF);  // Adiciona o deslocamento da página
    } else {
        logAccess(address, "translation miss", 4);
        return address;  // Endereço não traduzido
    }
}

void Memory::loadBIOS(const std::string& biosPath) {
    std::ifstream biosFile(biosPath, std::ios::binary);
    if (biosFile) {
        biosFile.read(reinterpret_cast<char*>(&bios[0]), BIOS_SIZE);
        biosFile.close();
    }
}

void Memory::enableDebugLogging(const std::string& logPath) {
    debugLoggingEnabled = true;
    debugLogFile.open(logPath, std::ios::out);
}

void Memory::logAccess(uint32_t address, const std::string& operation, uint32_t size) {
    if (debugLoggingEnabled && debugLogFile.is_open()) {
        debugLogFile << "Operation: " << operation 
                     << ", Address: " << std::hex << address
                     << ", Size: " << std::dec << size << std::endl;
    }
}

void Memory::printMemory(uint32_t startAddress, uint32_t size) {
    for (uint32_t i = startAddress; i < startAddress + size; i++) {
        uint32_t word = read32(i);
        std::cout << "Address: " << std::hex << i << " Value: " << word << std::endl;
    }
}