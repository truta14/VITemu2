#include <iostream>
#include <iomanip>
#include <string>
#include "cpu/cpu.hpp"
#include "memory/memory.hpp"

int main() {
    // Cria um objeto de memória
    Memory memory;

    // Teste de leitura e escrita simples na memória RAM
    uint32_t testAddress = 0x00010000; // Endereço de teste
    uint32_t testValue32 = 0xDEADBEEF; // Valor para testar a escrita de 32 bits
    uint16_t testValue16 = 0xABCD;     // Valor para testar a escrita de 16 bits
    uint8_t testValue8 = 0x12;         // Valor para testar a escrita de 8 bits

    // Teste de escrita e leitura de 32 bits
    std::cout << "Testando escrita e leitura de 32 bits...\n";
    memory.write32(testAddress, testValue32);
    uint32_t readValue32 = memory.read32(testAddress);
    std::cout << "Valor escrito: 0x" << std::hex << testValue32 << " | Valor lido: 0x" << readValue32 << "\n\n";

    // Teste de escrita e leitura de 16 bits
    std::cout << "Testando escrita e leitura de 16 bits...\n";
    memory.write16(testAddress, testValue16);
    uint16_t readValue16 = memory.read16(testAddress);
    std::cout << "Valor escrito: 0x" << std::hex << testValue16 << " | Valor lido: 0x" << readValue16 << "\n\n";

    // Teste de escrita e leitura de 8 bits
    std::cout << "Testando escrita e leitura de 8 bits...\n";
    memory.write8(testAddress, testValue8);
    uint8_t readValue8 = memory.read8(testAddress);
    std::cout << "Valor escrito: 0x" << std::hex << (int)testValue8 << " | Valor lido: 0x" << (int)readValue8 << "\n\n";

    // Carregar a BIOS (simulando que a BIOS já foi fornecida como arquivo binário)
    std::cout << "Carregando a BIOS...\n";
    memory.loadBIOS("E:\\Sony PlayStation 2 BIOS (E)(v1.6)(2001-10-04)[SCPH30004].bin");

    // Teste de cache de dados
    std::cout << "Testando cache de dados...\n";
    uint32_t cacheData = memory.cacheReadData(testAddress);
    std::cout << "Valor lido do cache de dados: 0x" << std::hex << cacheData << "\n\n";

    // Teste de cache de instruções
    std::cout << "Testando cache de instruções...\n";
    uint32_t cacheInst = memory.cacheReadInst(testAddress);
    std::cout << "Valor lido do cache de instruções: 0x" << std::hex << cacheInst << "\n\n";

    // Ativar logging de debug
    std::cout << "Ativando logging de debug...\n";
    memory.enableDebugLogging("memory_debug.log");

    // Fazer alguns acessos à memória e ao cache para gerar logs
    memory.read32(testAddress);  // Acesso de leitura
    memory.write16(testAddress, 0x1234);  // Acesso de escrita

    // Teste de impressão da memória
    std::cout << "Imprimindo a memória a partir do endereço 0x" << std::hex << testAddress << "...\n";
    memory.printMemory(testAddress, 64);  // Imprime 64 bytes da memória, começando de `testAddress`

    return 0;
}
