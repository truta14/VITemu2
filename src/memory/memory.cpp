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
    if (address < RAM_SIZE) {
        return ram[address / sizeof(uint32_t)];
    } else if (address >= BIOS_START_ADDRESS && address < BIOS_START_ADDRESS + BIOS_SIZE) {
        return bios[(address - BIOS_START_ADDRESS) / sizeof(uint32_t)];
    } else if (address >= SPRAM_START_ADDRESS && address < SPRAM_START_ADDRESS + SPRAM_SIZE) {
        uint32_t offset = address - SPRAM_START_ADDRESS;
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
    if (address < RAM_SIZE) {
        ram[address / sizeof(uint32_t)] = value;
    } else if (address >= SPRAM_START_ADDRESS && address < SPRAM_START_ADDRESS + SPRAM_SIZE) {
        uint32_t offset = address - SPRAM_START_ADDRESS;
        *(reinterpret_cast<uint32_t*>(&spram[offset])) = value;
    } else {
        logAccess(address, "write", 4);
    }
}

uint32_t Memory::cacheReadData(uint32_t address) {
    uint32_t index = (address / 32) % dataCache.size();
    uint32_t tag = address / (dataCache.size() * 32);

    if (dataCache[index].valid && dataCache[index].tag == tag) {
        return dataCache[index].data[(address / 4) % 8];
    }

    // Cache miss: carrega linha de dados
    dataCache[index].tag = tag;
    dataCache[index].valid = true;
    uint32_t baseAddr = address & ~0x1F;
    for (int i = 0; i < 8; ++i) {
        dataCache[index].data[i] = read32(baseAddr + i * 4);
    }
    return dataCache[index].data[(address / 4) % 8];
}

uint32_t Memory::cacheReadInst(uint32_t address) {
    uint32_t index = (address / 32) % instructionCache.size();
    uint32_t tag = address / (instructionCache.size() * 32);

    if (instructionCache[index].valid && instructionCache[index].tag == tag) {
        return instructionCache[index].data[(address / 4) % 8];
    }

    // Cache miss: carrega linha de instrução
    instructionCache[index].tag = tag;
    instructionCache[index].valid = true;
    uint32_t baseAddr = address & ~0x1F;
    for (int i = 0; i < 8; ++i) {
        instructionCache[index].data[i] = read32(baseAddr + i * 4);
    }
    return instructionCache[index].data[(address / 4) % 8];
}

void Memory::cacheWriteData(uint32_t address, uint32_t value) {
    uint32_t index = (address / 32) % dataCache.size();
    uint32_t tag = address / (dataCache.size() * 32);
    dataCache[index].tag = tag;
    dataCache[index].data[(address / 4) % 8] = value;
    dataCache[index].valid = true;
}

void Memory::cacheWriteInst(uint32_t address, uint32_t value) {
    uint32_t index = (address / 32) % instructionCache.size();
    uint32_t tag = address / (instructionCache.size() * 32);
    instructionCache[index].tag = tag;
    instructionCache[index].data[(address / 4) % 8] = value;
    instructionCache[index].valid = true;
}

// Método para imprimir um intervalo da memória
void Memory::printMemory(uint32_t startAddress, uint32_t size) {
    if (startAddress >= RAM_SIZE && 
        (startAddress < BIOS_START_ADDRESS || startAddress >= BIOS_START_ADDRESS + BIOS_SIZE)) {
        std::cerr << "Erro: Os endereços fornecidos estão fora dos limites da memória!" << std::endl;
        return;
    }

    uint32_t endAddress = startAddress + size;
    for (uint32_t address = startAddress; address < endAddress; address += sizeof(uint32_t)) {
        if (address >= RAM_SIZE && 
            (address < BIOS_START_ADDRESS || address >= BIOS_START_ADDRESS + BIOS_SIZE)) {
            std::cerr << "Erro: endereço de memória inválido para leitura: " 
                      << std::hex << address << std::endl;
            break;
        }

        uint32_t value = read32(address);
        std::cout << "Endereço: " << std::hex << address 
                  << " Valor: " << value << std::endl;
    }
}

// Método para carregar a BIOS do PS2 para a memória
void Memory::loadBIOS(const std::string& biosPath) {
    std::ifstream biosFile(biosPath, std::ios::binary);  // Abre o arquivo da BIOS no modo binário
    if (!biosFile) {
        std::cerr << "Erro ao abrir o arquivo da BIOS!" << std::endl;
        return;
    }

    // Verifica o tamanho da BIOS
    biosFile.seekg(0, std::ios::end);  // Vai para o final do arquivo
    std::streamsize biosSize = biosFile.tellg();  // Obtém o tamanho do arquivo
    if (biosSize != BIOS_SIZE) {
        std::cerr << "Erro: O tamanho da BIOS não é o esperado (4MB)!" << std::endl;
        return;
    }

    // Volta para o início do arquivo
    biosFile.seekg(0, std::ios::beg);

    // Lê o conteúdo da BIOS e carrega na memória BIOS
    biosFile.read(reinterpret_cast<char*>(&bios[0]), BIOS_SIZE);
    if (!biosFile) {
        std::cerr << "Erro ao ler o arquivo da BIOS!" << std::endl;
    } else {
        std::cout << "BIOS carregada com sucesso!" << std::endl;
    }
}

void Memory::enableDebugLogging(const std::string& logPath) {
    debugLoggingEnabled = true;
    debugLogFile.open(logPath, std::ios::out);
    if (!debugLogFile) {
        std::cerr << "Erro ao abrir arquivo de log!" << std::endl;
        debugLoggingEnabled = false;
    }
}

void Memory::logAccess(uint32_t address, const std::string& operation, uint32_t size) {
    if (debugLoggingEnabled && debugLogFile.is_open()) {
        debugLogFile << "Acesso: " << operation << " Endereço: " 
                     << std::hex << address << " Tamanho: " << size << " bytes\n";
    }
}

bool Memory::isValidAddress(uint32_t address, uint32_t size) {
    // Verifica se o endereço está dentro dos limites válidos de RAM, BIOS ou SPRAM
    if (address < RAM_SIZE) {
        return true;  // Dentro da RAM
    } else if (address >= BIOS_START_ADDRESS && address < BIOS_START_ADDRESS + BIOS_SIZE) {
        return true;  // Dentro da BIOS
    } else if (address >= SPRAM_START_ADDRESS && address < SPRAM_START_ADDRESS + SPRAM_SIZE) {
        return true;  // Dentro do SPRAM
    } else {
        // Endereço inválido
        std::cerr << "Erro: Endereço inválido de memória: " << std::hex << address << std::endl;
        return false;
    }
}

uint32_t Memory::translateAddress(uint32_t address) {
    // Simula a tradução de endereço (exemplo simples de mapeamento)
    // Aqui, para simplificação, mapeia-se endereços >= 0x80000000 para um novo espaço (poderia ser uma tabela de páginas mais complexa)
    if (address >= 0x80000000) {
        // Simulação de uma tradução de endereço para um novo espaço (exemplo simples)
        return address - 0x80000000;  // Apenas subtraímos um valor fixo, como exemplo
    }
    return address;  // Endereço não precisa de tradução
}
