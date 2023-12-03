#include "pq.h"
#include "node.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct ListElement {
    Node *tree;
    struct ListElement *next;
} ListElement;

struct PriorityQueue {
    ListElement *list;
};

static bool pq_less_than(ListElement *e1, ListElement *e2) {
    if (e1->tree->weight < e2->tree->weight) {
        return true;
    }
    if (e1->tree->weight == e2->tree->weight) {
        return e1->tree->symbol < e2->tree->symbol;
    }
    return false;
}

PriorityQueue *pq_create(void) {
    PriorityQueue *q = malloc(sizeof(PriorityQueue));
    if (q == NULL) {
        return NULL;
    }
    q->list = NULL;
    return q;
}

void pq_free(PriorityQueue **q) {
    if (q != NULL && *q != NULL) {
        ListElement *current = (*q)->list;
        while (current != NULL) {
            ListElement *temp = current;
            current = current->next;
            free(temp);
        }
        free(*q);
        *q = NULL;
    }
}

bool pq_is_empty(PriorityQueue *q) {
    return (q == NULL || q->list == NULL);
}

bool pq_size_is_1(PriorityQueue *q) {
    if (q != NULL && q->list != NULL) {
        return (q->list->next == NULL);
    }
    return false;
}

void enqueue(PriorityQueue *q, Node *tree) {
    ListElement *new_element = malloc(sizeof(ListElement));
    if (new_element == NULL) {
        // Handle memory allocation failure
        exit(1);
    }
    new_element->tree = tree;
    new_element->next = NULL;

    if (pq_is_empty(q)) {
        q->list = new_element;
    } else {
        if (pq_less_than(new_element, q->list)) {
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
}

Node *dequeue(PriorityQueue *q) {
    if (pq_is_empty(q)) {
        // Handle empty queue scenario
        exit(1);
    }
    ListElement *head = q->list;
    q->list = head->next;
    Node *tree = head->tree;
    free(head);
    return tree;
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
        node_print_tree(e->tree);
        e = e->next;
    }
    printf("=============================================\n");
}

