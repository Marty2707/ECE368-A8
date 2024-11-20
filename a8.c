#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

// Define constants
#define INF INT_MAX

// Data structures for the graph
typedef struct {
    int target;      // Target node
    int *weights;    // List of weights
    int period;      // Period of weights
} Edge;

typedef struct {
    Edge **edges;    // List of edges
    int edge_count;  // Number of edges
} Vertex;

Vertex *graph; // Graph representation
int V, N;      // Number of vertices and period

// Data structure for priority queue
typedef struct {
    int node;
    int cost;
    int step;
} State;

typedef struct {
    State *heap;
    int size;
    int capacity;
} PriorityQueue;

// Function to calculate the weight dynamically based on the step
int dynamic_weight(Edge *edge, int step) {
    return edge->weights[step % edge->period];
}

// Priority Queue Implementation
PriorityQueue *create_priority_queue(int capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->heap = malloc(capacity * sizeof(State));
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void push(PriorityQueue *pq, State s) {
    pq->heap[pq->size++] = s;
    int i = pq->size - 1;
    while (i > 0 && pq->heap[i].cost < pq->heap[(i - 1) / 2].cost) {
        State temp = pq->heap[i];
        pq->heap[i] = pq->heap[(i - 1) / 2];
        pq->heap[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

State pop(PriorityQueue *pq) {
    State min = pq->heap[0];
    pq->heap[0] = pq->heap[--pq->size];
    int i = 0;
    while (2 * i + 1 < pq->size) {
        int smallest = 2 * i + 1;
        if (2 * i + 2 < pq->size && pq->heap[2 * i + 2].cost < pq->heap[smallest].cost) {
            smallest = 2 * i + 2;
        }
        if (pq->heap[i].cost <= pq->heap[smallest].cost) break;
        State temp = pq->heap[i];
        pq->heap[i] = pq->heap[smallest];
        pq->heap[smallest] = temp;
        i = smallest;
    }
    return min;
}

bool is_empty(PriorityQueue *pq) {
    return pq->size == 0;
}

// Parse input file
void parse_input(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read number of vertices and period
    if (fscanf(file, "%d %d", &V, &N) != 2) {
        printf("Error: Invalid input format.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the graph
    graph = malloc(V * sizeof(Vertex));
    for (int i = 0; i < V; i++) {
        graph[i].edges = NULL;
        graph[i].edge_count = 0;
    }

    // Read edges
    int u, v;
    while (fscanf(file, "%d %d", &u, &v) != EOF) {
        int *weights = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            if (fscanf(file, "%d", &weights[i]) != 1) {
                printf("Error: Insufficient weights for edge (%d, %d).\n", u, v);
                exit(EXIT_FAILURE);
            }
        }
        Edge *edge = malloc(sizeof(Edge));
        edge->target = v;
        edge->weights = weights;
        edge->period = N;

        graph[u].edges = realloc(graph[u].edges, (graph[u].edge_count + 1) * sizeof(Edge *));
        graph[u].edges[graph[u].edge_count++] = edge;
    }

    fclose(file);
}

// Modified Dijkstra's Algorithm
void modified_dijkstra(int start, int end) {
    int **dist = malloc(V * sizeof(int *));
    bool **visited = malloc(V * sizeof(bool *));
    int *prev = malloc(V * sizeof(int));

    for (int i = 0; i < V; i++) {
        dist[i] = malloc(N * sizeof(int));
        visited[i] = malloc(N * sizeof(bool));
        for (int j = 0; j < N; j++) {
            dist[i][j] = INF;
            visited[i][j] = false;
        }
        prev[i] = -1;
    }

    PriorityQueue *pq = create_priority_queue(V * N);
    dist[start][0] = 0;
    push(pq, (State){start, 0, 0});

    while (!is_empty(pq)) {
        State current = pop(pq);
        int u = current.node, cost = current.cost, step = current.step;

        if (visited[u][step]) continue;
        visited[u][step] = true;

        if (u == end) {
            printf("Shortest path cost: %d\n", cost);
            goto cleanup;
        }

        for (int i = 0; i < graph[u].edge_count; i++) {
            Edge *edge = graph[u].edges[i];
            int v = edge->target;
            int weight = dynamic_weight(edge, step);
            int next_step = (step + 1) % N;

            if (cost + weight < dist[v][next_step]) {
                dist[v][next_step] = cost + weight;
                prev[v] = u;
                push(pq, (State){v, dist[v][next_step], next_step});
            }
        }
    }

    printf("No path found.\n");

cleanup:
    for (int i = 0; i < V; i++) {
        free(dist[i]);
        free(visited[i]);
    }
    free(dist);
    free(visited);
    free(prev);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    parse_input(argv[1]);

    printf("Enter queries (start end), one per line. Press Ctrl+D to end input:\n");
    int start, end;
    while (scanf("%d %d", &start, &end) != EOF) {
        if (start < 0 || start >= V || end < 0 || end >= V) {
            printf("Invalid query: (%d, %d). Nodes must be in range [0, %d).\n", start, end, V);
            continue;
        }
        modified_dijkstra(start, end);
    }

    return 0;
}
