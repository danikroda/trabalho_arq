#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para imprimir uma instrução em binário formatado
void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) printf(" "); // Agrupa em blocos de 4 bits
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
            case 0x20: printf("add $%d, $%d, $%d\n", rd, rs, rt); break;
            case 0x22: printf("sub $%d, $%d, $%d\n", rd, rs, rt); break;
            case 0x24: printf("and $%d, $%d, $%d\n", rd, rs, rt); break;
            case 0x25: printf("or $%d, $%d, $%d\n", rd, rs, rt); break;
            case 0x2A: printf("slt $%d, $%d, $%d\n", rd, rs, rt); break;
            default:   printf("Unknown R-type funct: 0x%02X\n", funct); break;
        }
    } else {
        switch (opcode) {
            case 0x08: printf("addi $%d, $%d, %d\n", rt, rs, (int16_t)immediate); break;
            case 0x0C: printf("andi $%d, $%d, 0x%04X\n", rt, rs, immediate); break;
            case 0x0D: printf("ori $%d, $%d, 0x%04X\n", rt, rs, immediate); break;
            case 0x0F: printf("lui $%d, 0x%04X\n", rt, immediate); break;
            case 0x23: printf("lw $%d, %d($%d)\n", rt, (int16_t)immediate, rs); break;
            case 0x2B: printf("sw $%d, %d($%d)\n", rt, (int16_t)immediate, rs); break;
            case 0x04: printf("beq $%d, $%d, %d\n", rs, rt, (int16_t)immediate); break;
            case 0x05: printf("bne $%d, $%d, %d\n", rs, rt, (int16_t)immediate); break;
            default:   printf("Unknown opcode: 0x%02X\n", opcode); break;
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

    // Leitura e decodificação linha por linha
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

        // Pergunta ao usuário se deseja continuar
        printf("Exibir a proxima instrucao? (s/n): ");
        char resposta;
        scanf(" %c", &resposta);

        if (resposta == 'n' || resposta == 'N') {
            printf("Execucao encerrada pelo usuario.\n");
            break;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}
