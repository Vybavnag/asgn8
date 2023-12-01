#include "pq.h"
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "node.h"


typedef struct ListElement ListElement;
struct ListElement {
    Node *tree;
    ListElement *next;
};

struct PriorityQueue {
    ListElement *list;
};

PriorityQueue *pq_create(void) {
    PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (q == NULL) {
        return NULL;
    }
    q->list = NULL;
    return q;
}

void pq_free(PriorityQueue **q) {
    if (q != NULL && *q != NULL) {
        free(*q);
        *q = NULL;
    }
}

bool pq_is_empty(PriorityQueue *q) {
    return q == NULL || q->list == NULL;
}

bool pq_size_is_1(PriorityQueue *q) {
    return q != NULL && q->list != NULL && q->list->next == NULL;
}

bool pq_less_than(ListElement *e1, ListElement *e2) {
    if (e1->tree->weight < e2->tree->weight) {
        return true;
    } else if (e1->tree->weight == e2->tree->weight) {
        return e1->tree->symbol < e2->tree->symbol;
    }
    return false;
}

void enqueue(PriorityQueue *q, Node *tree) {
    ListElement *new_element = (ListElement *)malloc(sizeof(ListElement));
    new_element->tree = tree;
    new_element->next = NULL;

    if (pq_is_empty(q)) {
        q->list = new_element;
    } else if (pq_less_than(new_element, q->list)) {
        new_element->next = q->list;
        q->list = new_element;
    } else {
        ListElement *current = q->list;
        while (current->next != NULL && !pq_less_than(new_element, current->next)) {
            current = current->next;
        }
        new_element->next = current->next;
        current->next = new_element;
    }
}

Node *dequeue(PriorityQueue *q) {
    if (pq_is_empty(q)) {
        exit(EXIT_FAILURE);
    }
    ListElement *temp = q->list;
    Node *node = temp->tree;
    q->list = q->list->next;
    free(temp);
    return node;
}

void pq_print(PriorityQueue *q) {
    assert(q != NULL);
    ListElement *e = q->list;
    int position = 1;
    while (e != NULL) {
        if (position++ == 1) {
            printf("=============================================\n");
        } else {
            printf("---------------------------------------------\n");
        }
        node_print_node(e->tree, '<', 2);
        e = e->next;
    }
}
