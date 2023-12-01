#include "node.h"
#include <stdlib.h>
#include <stdio.h>

Node *node_create(uint8_t symbol, uint32_t weight) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->symbol = symbol;
    new_node->weight = weight;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

void node_free(Node **node) {
    if (node != NULL && *node != NULL) {
        free(*node);
        *node = NULL;
    }
}

static void node_print_node(Node *tree, char ch, int indentation) {
    if (tree == NULL) {
        return;
    }

    node_print_node(tree->right, '/', indentation + 3);

    printf("%*cweight = %u", indentation, ch, tree->weight);
    if (tree->left == NULL && tree->right == NULL) {
        if (' ' <= tree->symbol && tree->symbol <= '~') {
            printf(", symbol = '%c'", tree->symbol);
        } else {
            printf(", symbol = 0x%02x", tree->symbol);
        }
    }
    printf("\n");

    node_print_node(tree->left, '\\', indentation + 3);
}

void node_print_tree(Node *tree) {
    node_print_node(tree, '<', 0);
}
