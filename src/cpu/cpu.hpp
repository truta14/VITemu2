#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>

class CPU {
public:
    CPU();
    ~CPU();

    // Métodos principais
    void reset();    // Reseta a CPU
    void execute();  // Executa as instruções (chama a execução de todas)

    // Método de depuração
    void printRegisters();

private:
    // Registradores principais (apenas alguns, como exemplo)
    uint32_t GPR[32];      // General Purpose Registers (GPRs)
    uint32_t PC;           // Program Counter
    uint32_t HI, LO;       // Registradores HI e LO usados para multiplicação/divisão

    // Métodos auxiliares
    void fetch();          // Busca a próxima instrução
    void decode();         // Decodifica a instrução
    void executeInstruction(); // Executa a instrução decodificada

    // Métodos para executar instruções com parâmetros
    void executeAddi(int rs, int rt, int immediate);  // ADDI
    void executeSubi(int rs, int rt, int immediate);  // SUBI
    void executeMul(int rs, int rt, int rd);          // MUL
    void executeAndi(int rs, int immediate, int rt);  // ANDI
    void executeOrri(int rs, int immediate, int rt);  // ORRI
    void executeXori(int rs, int immediate, int rt);  // XORI
};

#endif // CPU_HPP
