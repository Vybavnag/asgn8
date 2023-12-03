#include "bitreader.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void dehuff_decompress_file(FILE *fout, BitReader *inbuf) {
    uint8_t type1 = bit_read_uint8(inbuf);
    uint8_t type2 = bit_read_uint8(inbuf);
    uint32_t filesize = bit_read_uint32(inbuf);
    uint16_t num_leaves = bit_read_uint16(inbuf);

    assert(type1 == 'H');
    assert(type2 == 'C');

    int num_nodes = 2 * num_leaves - 1;
    Node *nodes[num_nodes];
    int node_count = 0;

    for (int i = 0; i < num_nodes; i++) {
        uint8_t bit = bit_read_bit(inbuf);
        Node *node;
        if (bit == 1) {
            uint8_t symbol = bit_read_uint8(inbuf);
            node = node_create(symbol, 0);
        } else {
            node = node_create(0, 0);
            node->right = nodes[--node_count];
            node->left = nodes[--node_count];
        }
        nodes[node_count++] = node;
    }

    Node *code_tree = nodes[0];
    for (uint32_t i = 0; i < filesize; i++) { // Corrected loop variable type to uint32_t
        Node *node = code_tree;
        while (node->left != NULL || node->right != NULL) {
            uint8_t bit = bit_read_bit(inbuf);
            node = (bit == 0) ? node->left : node->right;
        }
        fputc(node->symbol, fout);
    }

    // Free memory
    for (int i = 0; i < node_count; i++) {
        node_free(&nodes[i]);
    }
}

void print_help(void) { // Corrected function prototype
    printf("Huffman Coding Decompression\n");
    printf("Usage: dehuff [-i inputfile] [-o outputfile] [-h]\n");
    printf("  -i : Sets the name of the input file. Requires a filename as an argument.\n");
    printf("  -o : Sets the name of the output file. Requires a filename as an argument.\n");
    printf("  -h : Prints this help message.\n");
}

int main(int argc, char *argv[]) {
    char *input_filename = NULL;
    char *output_filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_filename = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_filename = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    if (!input_filename || !output_filename) {
        fprintf(stderr, "Input and output filenames are required.\n");
        print_help();
        return 1;
    }

    BitReader *inbuf = bit_read_open(input_filename);
    if (!inbuf) {
        perror("Failed to open input file");
        return 1;
    }

    FILE *fout = fopen(output_filename, "wb");
    if (!fout) {
        perror("Failed to open output file");
        bit_read_close(&inbuf);
        return 1;
    }

    dehuff_decompress_file(fout, inbuf);

    bit_read_close(&inbuf);
    fclose(fout);

    return 0;
}
