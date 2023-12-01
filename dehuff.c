#include "bitreader.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

void dehuff_decompress_file(FILE *fout, BitReader *inbuf) {
    uint8_t type1 = bit_read_uint8(inbuf);
    uint8_t type2 = bit_read_uint8(inbuf);
    uint32_t filesize = bit_read_uint32(inbuf);
    uint16_t num_leaves = bit_read_uint16(inbuf);

    assert(type1 == 'H');
    assert(type2 == 'C');

    uint16_t num_nodes = 2 * num_leaves - 1;
    Node *node, *stack[num_nodes];
    int stack_top = -1;

    for (int i = 0; i < num_nodes; i++) {
        uint8_t bit = bit_read_bit(inbuf);
        if (bit == 1) {
            uint8_t symbol = bit_read_uint8(inbuf);
            node = node_create(symbol, 0);
        } else {
            node = node_create(0, 0);
            node->right = stack[stack_top--];
            node->left = stack[stack_top--];
        }
        stack[++stack_top] = node;
    }

    Node *code_tree = stack[stack_top--];

    for (int i = 0; i < filesize; i++) {
        node = code_tree;
        while (node->left != NULL || node->right != NULL) {
            uint8_t bit = bit_read_bit(inbuf);
            node = (bit == 0) ? node->left : node->right;
        }
        fputc(node->symbol, fout);
    }
}

int main(int argc, char *argv[]) {
    char *inputFileName = NULL;
    char *outputFileName = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                inputFileName = optarg;
                break;
            case 'o':
                outputFileName = optarg;
                break;
            case 'h':
                printf("Usage: %s -i inputfile -o outputfile\n", argv[0]);
                return 0;
            default:
                fprintf(stderr, "Usage: %s -i inputfile -o outputfile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!inputFileName || !outputFileName) {
        fprintf(stderr, "Both input and output files must be specified\n");
        exit(EXIT_FAILURE);
    }

    FILE *inputFile = fopen(inputFileName, "rb");
    FILE *outputFile = fopen(outputFileName, "wb");

    if (!inputFile || !outputFile) {
        fprintf(stderr, "Error opening files\n");
        exit(EXIT_FAILURE);
    }

    BitReader *inbuf = bit_read_open(inputFileName);
    dehuff_decompress_file(outputFile, inbuf);

    fclose(inputFile);
    fclose(outputFile);
    bit_read_close(&inbuf);

    return 0;
}
