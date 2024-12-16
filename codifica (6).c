/* Gabriel Rosas 2210689 3WB */
/* Luisa Silveira 2210875 3WB */

// n de bits em um byte
#define BYTE 8
// tamanho do vetor struct compactadora *v
#define TAM 32

#include "codifica.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void compacta(FILE *arqTexto, FILE *arqBin, struct compactadora *v) {
    unsigned int byte = 0; // vai ser o nosso buffer
    int contbits = 0; 

    while (!feof(arqTexto)) {   // lemos o arquivo
      unsigned char letra = fgetc(arqTexto);  // pegamos a letra 
        for (int i = 0; i < 32; i++) {   // percorremos o vetor até achar um símbolo que corresponde a letra 
          
            if (v[i].simbolo == letra) {  // quando encontra, adiciona ao byte na posição correspondente 
                int tamanho = v[i].tamanho;
                contbits += tamanho;
                byte = byte << tamanho;
                byte|= v[i].codigo;

                while (contbits >= BYTE) {  // se o byte estiver cheio, com 8 ou mais bits, escreve. Isso se mantém até o contbits for menor que 8, não estiver cheio
                  
                    unsigned int novoCod= (byte >> (contbits - BYTE)) & 0xFF;
                    fwrite(&novoCod, 1, 1, arqBin);
                    contbits -= BYTE; // reduz de 8 o contbits
                }
      
            }
        }
    }

    // escrevendo o EOT, fazemos um processo semelhante ao anterior 
    byte = byte << v[31].tamanho;
    byte |= v[31].codigo; // Adiciona o código do EOT ao byte
    contbits += v[31].tamanho;
    while (contbits > 0) {   
        unsigned int novoCod = byte >> (contbits - BYTE);
        novoCod &= 0xFF;
        if (contbits < BYTE) {
            novoCod &= (0xFF >> (BYTE - contbits));  
        }
        fwrite(&novoCod, 1, 1, arqBin);
        contbits -= BYTE;
    }
}

void descompacta(FILE *arqBin, FILE *arqTexto, struct compactadora *v) {
    unsigned int buffer = 0; // armazena temporariamente os bits lidos 
    int contBits = 0; // contador para acompanhar quantos bits foram lidos

    while (!feof(arqBin)) { // Enquanto não chegarmos ao final do arquivo binário
        unsigned char hexa;

        fread(&hexa, 1, 1, arqBin); // Lê um byte do arquivo binário

        for (int j = 7; j >= 0; j--) { // Percorre cada bit do byte lido
            unsigned int novoCod = 0;
            novoCod |= (hexa >> j) & 1; // Extrai um bit do byte

            buffer |= (novoCod); // Adiciona o bit lido ao buffer
            contBits++; 

            for (int i = 0; i < TAM - 1; i++) { // Percorre o vetor de compactação
                if ((buffer != 0 && v[i].codigo == buffer && contBits == v[i].tamanho)) { // Se o buffer corresponder a um código no vetor e tiver o tamanho correto
                    fputc(v[i].simbolo, arqTexto); // Escreve o símbolo correspondente no arquivo de texto
                    buffer = 0; // Reinicia o buffer para armazenar o próximo código
                    contBits = 0; // Reinicia o contador de bits
                }
            }

            buffer <<= 1; // Desloca o buffer para a esquerda para liberar espaço para o próximo bit
        }
    }
}
