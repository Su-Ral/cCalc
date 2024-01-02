#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
struct eNode {
    float value;
    char opValue;
    int vtype;
    struct eNode* next;
    struct eNode* previous;
};

struct eNodes {
    struct eNode* head;
    struct eNode* tail;
};

struct eNodes create() {
    struct eNodes curr;
    curr.head = (struct eNode*)(malloc(sizeof(struct eNode)));
    curr.tail = (struct eNode*)(malloc(sizeof(struct eNode)));
    curr.head->next = curr.tail;
    curr.head->previous = NULL;
    curr.tail->next = NULL;
    curr.tail->previous = curr.head;
    return curr;
}

void insertAtBeg(struct eNodes* curr, int value) {
    struct eNode* nNode = (struct eNode*)(malloc(sizeof(struct eNode)));
    nNode->value = value;
    nNode->vtype = 0;
    nNode->previous = curr->head;
    nNode->next = curr->head->next;
    curr->head->next->previous = nNode;
    curr->head->next = nNode;
}

void insertAtEnd(struct eNodes* curr, int value) {
    struct eNode* nNode = (struct eNode*)(malloc(sizeof(struct eNode)));
    nNode->value = value;
    nNode->vtype = 0;
    nNode->next = curr->tail;
    nNode->previous = curr->tail->previous;
    curr->tail->previous->next = nNode;
    curr->tail->previous = nNode;
}

void insertAtBegOp(struct eNodes* curr, char value) {
    struct eNode* nNode = (struct eNode*)(malloc(sizeof(struct eNode)));
    nNode->opValue = value;
    nNode->vtype = 1;
    nNode->previous = curr->head;
    nNode->next = curr->head->next;
    curr->head->next->previous = nNode;
    curr->head->next = nNode;
}

void insertAtEndOp(struct eNodes* curr, char value) {
    struct eNode* nNode = (struct eNode*)(malloc(sizeof(struct eNode)));
    nNode->opValue = value;
    nNode->vtype = 1;
    nNode->next = curr->tail;
    nNode->previous = curr->tail->previous;
    curr->tail->previous->next = nNode;
    curr->tail->previous = nNode;
}

float expo(float x, float y) {
    float total = x;
    for (int i = 0; i < (int)(y - 1); i++) {
        total = total * x;
    }
    return total;
}
void print(struct eNodes* curr){
    struct eNode* ptr = curr->head;
    while(ptr != NULL){
    if(ptr->previous != NULL && ptr->next != NULL){
        if(ptr->vtype == 0){  
            printf("%.0f",ptr->value);
        }else{
            printf("%c",ptr->opValue);
        }
    }
    ptr = ptr->next;
    }
}
void collapsNode(struct eNodes* node, struct eNode* ptr, int spacing) {
    struct eNode* ptr2 = ptr;
    for (int i = 0; i < spacing; i++) {
        if(ptr2->next != NULL) ptr2 = ptr2->next;
    }
    ptr->previous->next = ptr2;
    ptr2->previous = ptr->previous; 
}

void parthBreakDown(struct eNodes* equation);
float breakDown(struct eNodes* equation) {
    struct eNode* ptr = equation->head;
    parthBreakDown(equation);

    // clear out exponents
    while (ptr->next != NULL) {
        if (ptr->opValue == 94 /*94 = ^*/) {
            ptr->previous->value = expo(ptr->previous->value, ptr->next->value);
            collapsNode(equation, ptr, 2);
            ptr = ptr->previous;
        }
        ptr = ptr->next;
    }
    ptr = equation->head;

    // clearing out multiplications
    while (ptr->next != NULL) {
        if (ptr->opValue == 42) {
            ptr->previous->value = ptr->previous->value * ptr->next->value;
            collapsNode(equation, ptr, 2);
            ptr = ptr->previous;
        }
        if (ptr->next != NULL) {
            if (ptr->opValue == 47) {
                ptr->previous->value = ptr->previous->value / ptr->next->value;
                collapsNode(equation, ptr, 2);
                ptr = ptr->previous;
            }
        }
        ptr = ptr->next;
    }
    ptr = equation->head;

    // collapsing addition/subtraction
    while (ptr->next != NULL) {
        if (ptr->opValue == '+') {
            ptr->previous->value = ptr->previous->value + ptr->next->value;
            collapsNode(equation, ptr, 2);
            ptr = ptr->previous;
        }
        if (ptr->opValue == '-') {
            ptr->previous->value = ptr->previous->value - ptr->next->value;
            collapsNode(equation, ptr, 2);
            ptr = ptr->previous;
        }
        ptr = ptr->next;
    }
    return equation->head->next->value;
}

void parthBreakDown(struct eNodes* equation) {
    int count = 0;
    int spacing = 1;
    float total;
    struct eNode* ptr = equation->head->next;
    struct eNode* parptr = ptr;
    struct eNodes partial = create();
    while (ptr->next != NULL) {
        if (ptr->opValue == 40) {
            parptr = ptr->next;
            while (parptr != NULL && parptr->opValue != ')' ) {
                if (parptr->vtype == 0) { // if a number node
                    insertAtEnd(&partial, parptr->value);
                } else {
                    insertAtEndOp(&partial, parptr->opValue);
                }
                spacing++;
                parptr = parptr->next;
            }
            total = breakDown(&partial);
            ptr->value = total;
            ptr->vtype = 0;
            collapsNode(equation, ptr->next, spacing);
        }
        count++;
        ptr = ptr->next;
    }
}

int main() {
    struct eNodes equation = create();
    char userInput[20] = "4^(2(1+1)^2)";
    int count = 0;
    int currNum = 0;
    bool doubleOp = true;
    bool lastPar = false;

    while (userInput[count] != '\0') {
        if (userInput[count] != ' ') {
            if (((int)(userInput[count])) >= 48 && ((int)(userInput[count])) <= 57) {
                currNum = ((int)(userInput[count]) - 48) + (currNum * 10);
                doubleOp = false;
            } else {
                if (userInput[count] == '(' && doubleOp == true) { //Checks if there is a number or operator behind (
                    insertAtEndOp(&equation, userInput[count]);
                } else if (userInput[count] == ')' && (userInput[count + 1] < 48 || userInput[count + 1] > 57)) { 
                    insertAtEnd(&equation, currNum);
                    insertAtEndOp(&equation, userInput[count]);
                    insertAtEndOp(&equation, userInput[count + 1]);
                    count++;
                    if (userInput[count + 1] == '\0') lastPar = true;
                } else if (userInput[count] == ')') {
                    insertAtEnd(&equation, currNum);
                    insertAtEndOp(&equation, userInput[count]);
                    insertAtEndOp(&equation, '*');
                } else if (userInput[count] == '(') {  
                    insertAtEnd(&equation, currNum);
                    insertAtEndOp(&equation, '*');
                    insertAtEndOp(&equation, userInput[count]);
                } else {
                    lastPar = false;
                    insertAtEnd(&equation, currNum);
                    insertAtEndOp(&equation, userInput[count]);
                }
                doubleOp = true;
                currNum = 0;
            }
        }
        
        count++;
        print(&equation);
        printf("\n");
    }
    if (lastPar == false) insertAtEnd(&equation, currNum);
    printf("%f",breakDown(&equation));

    return 0;
}
