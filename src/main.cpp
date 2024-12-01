#include <iostream>
#include "cpu/cpu.hpp"

int main() {
    // Criação da instância da CPU
    CPU cpu;

    // Testa o reset da CPU
    cpu.reset();
    
    // Testa a impressão dos registradores após o reset
    std::cout << "Registradores após reset:" << std::endl;
    cpu.printRegisters();
    
    // Testa a execução de um ciclo de instrução (ADDI e SUBI)
    std::cout << "\nExecutando ciclo de instrução..." << std::endl;
    cpu.execute();

    // Testa a impressão dos registradores após a execução da instrução
    std::cout << "\nRegistradores após execução:" << std::endl;
    cpu.printRegisters();

    return 0;
}