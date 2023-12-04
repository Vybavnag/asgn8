#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Code {
    uint64_t code;
    uint8_t code_length;
} Code;

uint32_t fill_histogram(FILE *fin, uint32_t *histogram) {
    uint32_t filesize = 0;
    int byte;
    while ((byte = fgetc(fin)) != EOF) {
        histogram[byte]++;
        filesize++;
    }
    histogram[0x00]++;
    histogram[0xff]++;
    return filesize;
}

Node *create_tree(uint32_t *histogram, uint16_t *num_leaves) {
    PriorityQueue *pq = pq_create();
    *num_leaves = 0;

    for (int i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            enqueue(pq, node_create((uint8_t)i, histogram[i]));
            (*num_leaves)++;
        }
    }

    while (!pq_size_is_1(pq)) {
        Node *left = dequeue(pq);
        Node *right = dequeue(pq);
        Node *parent = node_create(0, left->weight + right->weight);
        parent->left = left;
        parent->right = right;
        enqueue(pq, parent);
    }

    Node *root = dequeue(pq);
    pq_free(&pq); 
    return root;
}

void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    if (node->left != NULL || node->right != NULL) {
        fill_code_table(code_table, node->left, code, code_length + 1);
        code |= (uint64_t)1 << code_length;
        fill_code_table(code_table, node->right, code, code_length + 1);
    } else {
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
    }
}

void huff_write_tree(BitWriter *outbuf, Node *node) {
    if (node->left == NULL && node->right == NULL) {
        bit_write_bit(outbuf, 1);
        bit_write_uint8(outbuf, node->symbol);
    } else {
        bit_write_bit(outbuf, 0);
        huff_write_tree(outbuf, node->left);
        huff_write_tree(outbuf, node->right);
    }
}

void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves, Node *code_tree, Code *code_table) {
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    bit_write_uint32(outbuf, filesize);
    bit_write_uint16(outbuf, num_leaves);
    huff_write_tree(outbuf, code_tree);

    rewind(fin);
    int byte;
    while ((byte = fgetc(fin)) != EOF) {
        uint64_t code = code_table[byte].code;
        uint8_t code_length = code_table[byte].code_length;
        for (int i = 0; i < code_length; i++) {
            bit_write_bit(outbuf, (code >> i) & 1);
        }
    }
}

void print_help(void) {
    printf("Huffman Coding Compression\n");
    printf("Usage: huff [-i inputfile] [-o outputfile] [-h]\n");
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

    FILE *fin = fopen(input_filename, "rb");
    if (!fin) {
        perror("Failed to open input file");
        return 1;
    }

    BitWriter *outbuf = bit_write_open(output_filename);
    if (!outbuf) {
        perror("Failed to open output file");
        fclose(fin);
        return 1;
    }

    uint32_t histogram[256] = {0};
    uint32_t filesize = fill_histogram(fin, histogram);

    uint16_t num_leaves;
    Node *code_tree = create_tree(histogram, &num_leaves);

    Code code_table[256];
    memset(code_table, 0, sizeof(code_table));
    fill_code_table(code_table, code_tree, 0, 0);

    huff_compress_file(outbuf, fin, filesize, num_leaves, code_tree, code_table);

    bit_write_close(&outbuf);
    fclose(fin);

    node_free(&code_tree); 

    return 0;
}
