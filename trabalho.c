#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_REGISTERS 32

// Simula os registradores MIPS
int32_t registers[NUM_REGISTERS] = {0};

// Nomes dos registradores MIPS
const char *register_names[NUM_REGISTERS] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

// Função para imprimir uma instrução em binário formatado
void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) printf(" "); // Agrupa em blocos de 4 bits
    }
    printf("\n");
}

// Exibe os valores dos registradores
void print_registers() {
    printf("\nEstado atual dos registradores:\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        printf("%s: %d\t", register_names[i], registers[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n");
}

// Função que decodifica uma instrução MIPS em formato binário
void decode_mips_instruction(uint32_t instruction) {
    uint8_t opcode     = (instruction >> 26) & 0x3F;
    uint8_t rs         = (instruction >> 21) & 0x1F;
    uint8_t rt         = (instruction >> 16) & 0x1F;
    uint8_t rd         = (instruction >> 11) & 0x1F;
    uint8_t shamt      = (instruction >> 6)  & 0x1F;
    uint8_t funct      = instruction & 0x3F;
    uint16_t immediate = instruction & 0xFFFF;
    uint32_t address   = instruction & 0x3FFFFFF;

    if (opcode == 0) {
        switch (funct) {
            case 0x20: // add
                registers[rd] = registers[rs] + registers[rt];
                printf("add %s, %s, %s\n", register_names[rd], register_names[rs], register_names[rt]);
                break;
            case 0x22: // sub
                registers[rd] = registers[rs] - registers[rt];
                printf("sub %s, %s, %s\n", register_names[rd], register_names[rs], register_names[rt]);
                break;
            case 0x24: // and
                registers[rd] = registers[rs] & registers[rt];
                printf("and %s, %s, %s\n", register_names[rd], register_names[rs], register_names[rt]);
                break;
            case 0x25: // or
                registers[rd] = registers[rs] | registers[rt];
                printf("or %s, %s, %s\n", register_names[rd], register_names[rs], register_names[rt]);
                break;
            case 0x2A: // slt
                registers[rd] = (registers[rs] < registers[rt]) ? 1 : 0;
                printf("slt %s, %s, %s\n", register_names[rd], register_names[rs], register_names[rt]);
                break;
            default:
                printf("Unknown R-type funct: 0x%02X\n", funct);
                break;
        }
    } else {
        switch (opcode) {
            case 0x08: // addi
                registers[rt] = registers[rs] + (int16_t)immediate;
                printf("addi %s, %s, %d\n", register_names[rt], register_names[rs], (int16_t)immediate);
                break;
            case 0x0C: // andi
                registers[rt] = registers[rs] & immediate;
                printf("andi %s, %s, 0x%04X\n", register_names[rt], register_names[rs], immediate);
                break;
            case 0x0D: // ori
                registers[rt] = registers[rs] | immediate;
                printf("ori %s, %s, 0x%04X\n", register_names[rt], register_names[rs], immediate);
                break;
            case 0x0F: // lui
                registers[rt] = immediate << 16;
                printf("lui %s, 0x%04X\n", register_names[rt], immediate);
                break;
            case 0x23: // lw
                printf("lw %s, %d(%s)\n", register_names[rt], (int16_t)immediate, register_names[rs]);
                break;
            case 0x2B: // sw
                printf("sw %s, %d(%s)\n", register_names[rt], (int16_t)immediate, register_names[rs]);
                break;
            case 0x04: // beq
                if (registers[rs] == registers[rt]) {
                    printf("beq %s, %s, %d (branch taken)\n", register_names[rs], register_names[rt], (int16_t)immediate);
                } else {
                    printf("beq %s, %s, %d (branch not taken)\n", register_names[rs], register_names[rt], (int16_t)immediate);
                }
                break;
            case 0x05: // bne
                if (registers[rs] != registers[rt]) {
                    printf("bne %s, %s, %d (branch taken)\n", register_names[rs], register_names[rt], (int16_t)immediate);
                } else {
                    printf("bne %s, %s, %d (branch not taken)\n", register_names[rs], register_names[rt], (int16_t)immediate);
                }
                break;
            default:
                printf("Unknown opcode: 0x%02X\n", opcode);
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_instrucoes_binarias>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    char linha[50]; // Buffer para ler cada linha do arquivo
    uint32_t instruction;
    int linhaAtual = 0;

    printf("Escolha uma opcao:\n");
    printf("1 - Exibir instrucoes uma por uma\n");
    printf("2 - Exibir todas as instrucoes de uma vez\n");
    printf("Opcao: ");
    int opcao;
    scanf("%d", &opcao);
    getchar(); // Consumir o caractere de nova linha

    switch (opcao) {
        case 1:
            while (fgets(linha, sizeof(linha), file)) {
                linhaAtual++;

                // Remove espaços e verifica se a linha tem exatamente 32 bits
                char *ptr = linha;
                int bit_count = 0;
                uint32_t value = 0;

                while (*ptr) {
                    if (isspace(*ptr)) {
                        ptr++;
                        continue;
                    }
                    if (*ptr != '0' && *ptr != '1') {
                        fprintf(stderr, "Linha %d: Caracter invalido: %c\n", linhaAtual, *ptr);
                        fclose(file);
                        return EXIT_FAILURE;
                    }
                    value = (value << 1) | (*ptr - '0');
                    bit_count++;
                    ptr++;
                }

                if (bit_count != 32) {
                    fprintf(stderr, "Linha %d: Instrucao invalida, deve conter exatamente 32 bits\n", linhaAtual);
                    continue;
                }

                instruction = value;

                printf("Instrucao MIPS em binario: ");
                print_binary(instruction);
                decode_mips_instruction(instruction);

                print_registers(); // Exibe o estado dos registradores

                // Pergunta ao usuário se deseja continuar
                printf("Exibir a proxima instrucao? (s/n): ");
                char resposta;
                scanf(" %c", &resposta);

                if (resposta == 'n' || resposta == 'N') {
                    printf("Execucao encerrada pelo usuario.\n");
                    break;
                }
            }
            break;

        case 2:
            while (fgets(linha, sizeof(linha), file)) {
                linhaAtual++;

                // Remove espaços e verifica se a linha tem exatamente 32 bits
                char *ptr = linha;
                int bit_count = 0;
                uint32_t value = 0;

                while (*ptr) {
                    if (isspace(*ptr)) {
                        ptr++;
                        continue;
                    }
                    if (*ptr != '0' && *ptr != '1') {
                        fprintf(stderr, "Linha %d: Caracter invalido: %c\n", linhaAtual, *ptr);
                        fclose(file);
                        return EXIT_FAILURE;
                    }
                    value = (value << 1) | (*ptr - '0');
                    bit_count++;
                    ptr++;
                }

                if (bit_count != 32) {
                    fprintf(stderr, "Linha %d: Instrucao invalida, deve conter exatamente 32 bits\n", linhaAtual);
                    continue;
                }

                instruction = value;

                printf("Instrucao MIPS em binario: ");
                print_binary(instruction);
                decode_mips_instruction(instruction);

                print_registers(); // Exibe o estado dos registradores
            }
            break;

        default:
            printf("Opcao invalida. Encerrando.\n");
            break;
    }

    fclose(file);
    return EXIT_SUCCESS;
}
