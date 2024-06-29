#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define N 3
#define MAX_STATES 362880 // 9!

typedef struct {
    int board[N][N];
    int blank_row;
    int blank_col;
    int cost;
} State;

typedef struct {
    State* state;
    int priority;
} PQNode;

typedef struct {
    PQNode* nodes[MAX_STATES];
    int size;
} PriorityQueue;

State* initializeState(int initial[N][N]) {
    State* state = (State*)malloc(sizeof(State));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            state->board[i][j] = initial[i][j];
            if (initial[i][j] == 0) {
                state->blank_row = i;
                state->blank_col = j;
            }
        }
    }
    state->cost = 0;
    return state;
}

bool isGoalState(State* state) {
    int count = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (state->board[i][j] != count % (N*N)) {
                return false;
            }
            count++;
        }
    }
    return true;
}

int calculateHammingPriority(State* state) {
    int priority = 0;
    int count = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (state->board[i][j] != count % (N*N) && state->board[i][j] != 0) {
                priority++;
            }
            count++;
        }
    }
    return priority;
}

int calculateManhattanDistance(int value, int row, int col) {
    int goal_row = (value - 1) / N;
    int goal_col = (value - 1) % N;
    return abs(goal_row - row) + abs(goal_col - col);
}

int calculateManhattanPriority(State* state) {
    int priority = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (state->board[i][j] != 0) {
                priority += calculateManhattanDistance(state->board[i][j], i, j);
            }
        }
    }
    return priority;
}

void printBoard(int board[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    return pq;
}

void push(PriorityQueue* pq, State* state, int priority) {
    PQNode* newNode = (PQNode*)malloc(sizeof(PQNode));
    newNode->state = state;
    newNode->priority = priority;
    pq->nodes[pq->size++] = newNode;
}

State* pop(PriorityQueue* pq) {
    int highestPriority = INT_MAX;
    int idx = -1;
    for (int i = 0; i < pq->size; i++) {
        if (pq->nodes[i]->priority < highestPriority) {
            highestPriority = pq->nodes[i]->priority;
            idx = i;
        }
    }
    State* state = pq->nodes[idx]->state;
    free(pq->nodes[idx]);
    for (int i = idx; i < pq->size - 1; i++) {
        pq->nodes[i] = pq->nodes[i + 1];
    }
    pq->size--;
    return state;
}

bool isEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

void aStarSearch(State* initialState) {
    PriorityQueue* openSet = createPriorityQueue();
    push(openSet, initialState, initialState->cost);

    while (!isEmpty(openSet)) {
        State* currentState = pop(openSet);

        if (isGoalState(currentState)) {
            printf("Goal state reached!\n");
            printf("Total cost: %d\n", currentState->cost);
            printf("Steps to reach the goal state:\n");
            printBoard(currentState->board);
            return;
        }

        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if ((dr == 0 && dc == 0) || (dr != 0 && dc != 0)) {
                    continue;
                }
                int new_blank_row = currentState->blank_row + dr;
                int new_blank_col = currentState->blank_col + dc;
                if (new_blank_row >= 0 && new_blank_row < N && new_blank_col >= 0 && new_blank_col < N) {
                    State* successor = (State*)malloc(sizeof(State));
                    *successor = *currentState;
                    swap(&successor->board[currentState->blank_row][currentState->blank_col],
                         &successor->board[new_blank_row][new_blank_col]);
                    successor->blank_row = new_blank_row;
                    successor->blank_col = new_blank_col;
                    successor->cost = currentState->cost + 1;
                    int hammingPriority = calculateHammingPriority(successor);
                    int manhattanPriority = calculateManhattanPriority(successor);
                    int totalPriority = successor->cost + manhattanPriority;
                    push(openSet, successor, totalPriority);
                }
            }
        }

        free(currentState);
    }

    printf("Goal state could not be reached!\n");

    free(openSet);
}

int main() {
    int initial[N][N] = {
        {1, 2, 3},
        {4, 0, 5},
                {6, 7, 8}
    };

    State* initialState = initializeState(initial);

    if (isGoalState(initialState)) {
        printf("Initial state is the goal state!\n");
        return 0;
    }

    printf("Initial state:\n");
    printBoard(initialState->board);

    int hammingPriority = calculateHammingPriority(initialState);
    printf("Hamming Priority: %d\n", hammingPriority);

    int manhattanPriority = calculateManhattanPriority(initialState);
    printf("Manhattan Priority: %d\n", manhattanPriority);

    aStarSearch(initialState);

    free(initialState);
    return 0;
}
