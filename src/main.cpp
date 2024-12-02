#include <iostream>
#include <iomanip>
#include <string>
#include "memory/memory.hpp"

int main() {
    // Criação da instância de memória
    Memory memory;

    // Habilitar logging de debug para registrar as operações
    memory.enableDebugLogging("memory_log.txt");

    // Carregar o BIOS
    memory.loadBIOS("E:\\Sony PlayStation 2 BIOS (E)(v1.6)(2001-10-04)[SCPH30004].bin");

    // Teste de leitura e escrita na RAM
    uint32_t ramAddress = 0x1000;  // Um endereço de teste na RAM
    uint32_t writeValue = 0x12345678; // Valor para escrever
    std::cout << "Escrevendo na RAM, endereço: " << std::hex << ramAddress << ", valor: " << writeValue << std::endl;
    memory.write32(ramAddress, writeValue);  // Escrever na RAM

    uint32_t readValue = memory.read32(ramAddress);  // Ler o valor de volta
    std::cout << "Valor lido da RAM, endereço: " << std::hex << ramAddress << ", valor: " << readValue << std::endl;

    // Teste de leitura e escrita no Scratchpad RAM
    uint32_t spramAddress = 0x70001000;  // Endereço de teste no Scratchpad RAM
    uint32_t spramWriteValue = 0xAABBCCDD;
    std::cout << "Escrevendo no Scratchpad RAM, endereço: " << std::hex << spramAddress << ", valor: " << spramWriteValue << std::endl;
    memory.write32(spramAddress, spramWriteValue);  // Escrever no Scratchpad RAM

    uint32_t spramReadValue = memory.read32(spramAddress);  // Ler o valor de volta
    std::cout << "Valor lido do Scratchpad RAM, endereço: " << std::hex << spramAddress << ", valor: " << spramReadValue << std::endl;

    // Teste de acesso ao cache de dados
    uint32_t cacheDataAddress = 0x1000;  // Endereço para acessar a cache de dados
    std::cout << "Acessando a cache de dados, endereço: " << std::hex << cacheDataAddress << std::endl;
    uint32_t cacheDataValue = memory.cacheReadData(cacheDataAddress);
    std::cout << "Valor lido da cache de dados: " << cacheDataValue << std::endl;

    // Teste de acesso ao cache de instruções
    uint32_t cacheInstAddress = 0x2000;  // Endereço para acessar a cache de instruções
    std::cout << "Acessando a cache de instruções, endereço: " << std::hex << cacheInstAddress << std::endl;
    uint32_t cacheInstValue = memory.cacheReadInst(cacheInstAddress);
    std::cout << "Valor lido da cache de instruções: " << cacheInstValue << std::endl;

    // Imprimir valores de memória
    uint32_t startAddress = 0x1000;
    uint32_t size = 16;  // Imprimir 16 bytes de memória
    std::cout << "Imprimindo a memória a partir do endereço: " << std::hex << startAddress << std::endl;
    memory.printMemory(startAddress, size);

    // Verificar os logs de acesso
    std::cout << "Acessos de memória registrados em 'memory_log.txt'." << std::endl;

    return 0;
}
