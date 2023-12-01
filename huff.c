#include "bitwriter.h"
#include "bitreader.h"
#include "node.h"
#include "pq.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

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
            enqueue(pq, node_create(i, histogram[i]));
            (*num_leaves)++;
        }
    }

    while (!pq_size_is_1(pq)) {
        Node *left = dequeue(pq);
        Node *right = dequeue(pq);

        Node *merged_node = node_create(-1, left->weight + right->weight);
        merged_node->left = left;
        merged_node->right = right;

        enqueue(pq, merged_node);
    }

    return dequeue(pq);
}

void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    if (node->left != NULL || node->right != NULL) {
        fill_code_table(code_table, node->left, code, code_length + 1);
        fill_code_table(code_table, node->right, code | ((uint64_t)1 << code_length), code_length + 1);
    } else {
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
    }
}

void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves, Node *code_tree, Code *code_table) {
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    bit_write_uint32(outbuf, filesize);
    bit_write_uint16(outbuf, num_leaves);

    // huff_write_tree(outbuf, code_tree); // Implement this function to write the tree

    rewind(fin);
    int b;
    while ((b = fgetc(fin)) != EOF) {
        uint64_t code = code_table[b].code;
        uint8_t code_length = code_table[b].code_length;
        for (int i = 0; i < code_length; i++) {
            bit_write_bit(outbuf, (code >> i) & 1);
        }
    }
}

void huffman_compress(const char *inputFileName, const char *outputFileName) {
    FILE *inputFile = fopen(inputFileName, "rb");
    uint32_t histogram[256] = {0};
    uint32_t fileSize = fill_histogram(inputFile, histogram);

    uint16_t numLeaves = 0;
    Node *root = create_tree(histogram, &numLeaves);

    Code codeTable[256] = {0};
    fill_code_table(codeTable, root, 0, 0);

    BitWriter *outputFile = bit_write_open(outputFileName);
    huff_compress_file(outputFile, inputFile, fileSize, numLeaves, root, codeTable);

    fclose(inputFile);
    bit_write_close(&outputFile);
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

    huffman_compress(inputFileName, outputFileName);

    return 0;
}
