#include "cpu.hpp"
#include <iostream>
#include <iomanip>

// Construtor e destruidor
CPU::CPU() {
    reset();
}

CPU::~CPU() {
    // Destruidor, por enquanto não precisa fazer nada
}

// Reseta os registradores e o estado da CPU
void CPU::reset() {
    for (int i = 0; i < 32; i++) {
        GPR[i] = 0; // Zera todos os registradores
    }
    PC = 0x80000000; // Endereço de memória inicial para o PS2
    HI = LO = 0;
}

// Método principal de execução
void CPU::execute() {
    fetch();
    decode();
    executeInstruction();
}

// Busca a próxima instrução
void CPU::fetch() {
    std::cout << "Buscando instrução no endereço: " << std::hex << PC << std::endl;
}

// Decodifica a instrução
void CPU::decode() {
    std::cout << "Decodificando instrução..." << std::endl;
}

// Executa as instruções passando parâmetros
void CPU::executeInstruction() {
    // Exemplo de chamada para cada método de instrução com parâmetros
    executeAddi(1, 2, 5);  // R2 = R1 + 5
    executeSubi(2, 3, 3);  // R3 = R2 - 3
    executeMul(2, 3, 4);   // R4 = R2 * R3
    executeAndi(2, 0xFF, 5);  // R5 = R2 & 0xFF
    executeOrri(2, 0x3, 6);   // R6 = R2 | 0x3
    executeXori(2, 0xF, 7);   // R7 = R2 ^ 0xF
}

// Executa a instrução ADDI com parâmetros
void CPU::executeAddi(int rs, int rt, int immediate) {
    std::cout << "Executando a instrução ADDI: R" << rt << " = R" << rs << " + " << immediate << std::endl;
    GPR[rt] = GPR[rs] + immediate;  // GPR[rt] recebe o valor de GPR[rs] + immediate
    std::cout << "R" << rt << " agora é: " << std::hex << GPR[rt] << std::endl;
}

// Executa a instrução SUBI com parâmetros
void CPU::executeSubi(int rs, int rt, int immediate) {
    std::cout << "Executando a instrução SUBI: R" << rt << " = R" << rs << " - " << immediate << std::endl;
    GPR[rt] = GPR[rs] - immediate;  // Subtrai o imediato de GPR[rs] e armazena em GPR[rt]
    std::cout << "R" << rt << " agora é: " << std::hex << GPR[rt] << std::endl;
}

// Executa a instrução MUL com parâmetros
void CPU::executeMul(int rs, int rt, int rd) {
    std::cout << "Executando a instrução MUL: R" << rd << " = R" << rs << " * R" << rt << std::endl;
    GPR[rd] = GPR[rs] * GPR[rt];  // Multiplica GPR[rs] por GPR[rt] e armazena em GPR[rd]
    std::cout << "R" << rd << " agora é: " << std::hex << GPR[rd] << std::endl;
}

// Executa a instrução ANDI com parâmetros
void CPU::executeAndi(int rs, int immediate, int rt) {
    std::cout << "Executando a instrução ANDI: R" << rt << " = R" << rs << " & " << std::hex << immediate << std::endl;
    GPR[rt] = GPR[rs] & immediate;  // Realiza a operação AND entre GPR[rs] e o imediato e armazena em GPR[rt]
    std::cout << "R" << rt << " agora é: " << std::hex << GPR[rt] << std::endl;
}

// Executa a instrução ORRI com parâmetros
void CPU::executeOrri(int rs, int immediate, int rt) {
    std::cout << "Executando a instrução ORRI: R" << rt << " = R" << rs << " | " << std::hex << immediate << std::endl;
    GPR[rt] = GPR[rs] | immediate;  // Realiza a operação OR entre GPR[rs] e o imediato e armazena em GPR[rt]
    std::cout << "R" << rt << " agora é: " << std::hex << GPR[rt] << std::endl;
}

// Executa a instrução XORI com parâmetros
void CPU::executeXori(int rs, int immediate, int rt) {
    std::cout << "Executando a instrução XORI: R" << rt << " = R" << rs << " ^ " << std::hex << immediate << std::endl;
    GPR[rt] = GPR[rs] ^ immediate;  // Realiza a operação XOR entre GPR[rs] e o imediato e armazena em GPR[rt]
    std::cout << "R" << rt << " agora é: " << std::hex << GPR[rt] << std::endl;
}

// Método de depuração para imprimir os registradores
void CPU::printRegisters() {
    std::cout << "Registradores GPR: \n";
    for (int i = 0; i < 32; i++) {
        std::cout << "R" << i << ": " << std::setw(8) << std::hex << GPR[i] << std::endl;
    }
    std::cout << "PC: " << std::setw(8) << std::hex << PC << std::endl;
    std::cout << "HI: " << std::setw(8) << std::hex << HI << std::endl;
    std::cout << "LO: " << std::setw(8) << std::hex << LO << std::endl;
}
