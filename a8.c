#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

// Define constants
#define INF INT_MAX
#define MAX_NODES 100000 // Maximum nodes in the time-expanded graph

// Graph Structures
typedef struct {
    int target;      // Target node
    int *weights;    // List of weights
    int period;      // Period of weights
} Edge;

typedef struct {
    Edge **edges;    // List of edges
    int edge_count;  // Number of edges
} Vertex;

// Priority Queue
typedef struct {
    int node;
    int time;
    int cost;
} State;

typedef struct {
    State *heap;
    int size;
    int capacity;
} PriorityQueue;

// Globals
Vertex *graph;
int V, N;

// Priority Queue Functions
PriorityQueue *create_priority_queue(int capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (!pq) {
        fprintf(stderr, "Error: Memory allocation failed for priority queue.\n");
        exit(EXIT_FAILURE);
    }
    pq->heap = malloc(capacity * sizeof(State));
    if (!pq->heap) {
        fprintf(stderr, "Error: Memory allocation failed for priority queue heap.\n");
        exit(EXIT_FAILURE);
    }
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void push(PriorityQueue *pq, State s) {
    if (pq->size >= pq->capacity) {
        fprintf(stderr, "Error: Priority queue overflow.\n");
        exit(EXIT_FAILURE);
    }
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
    if (pq->size == 0) {
        fprintf(stderr, "Error: Priority queue underflow.\n");
        exit(EXIT_FAILURE);
    }
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

// Graph Functions
void parse_input(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d %d", &V, &N) != 2) {
        fprintf(stderr, "Error: Invalid input format.\n");
        exit(EXIT_FAILURE);
    }

    if (V * N > MAX_NODES) {
        fprintf(stderr, "Error: Too many nodes for the graph structure. Increase MAX_NODES.\n");
        exit(EXIT_FAILURE);
    }

    graph = malloc(V * sizeof(Vertex));
    if (!graph) {
        fprintf(stderr, "Error: Memory allocation failed for graph.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < V; i++) {
        graph[i].edges = NULL;
        graph[i].edge_count = 0;
    }

    int u, v;
    while (fscanf(file, "%d %d", &u, &v) != EOF) {
        if (u < 0 || u >= V || v < 0 || v >= V) {
            fprintf(stderr, "Error: Invalid edge (%d, %d).\n", u, v);
            exit(EXIT_FAILURE);
        }

        int *weights = malloc(N * sizeof(int));
        if (!weights) {
            fprintf(stderr, "Error: Memory allocation failed for edge weights.\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < N; i++) {
            if (fscanf(file, "%d", &weights[i]) != 1) {
                fprintf(stderr, "Error: Insufficient weights for edge (%d, %d).\n", u, v);
                exit(EXIT_FAILURE);
            }
        }

        Edge *edge = malloc(sizeof(Edge));
        if (!edge) {
            fprintf(stderr, "Error: Memory allocation failed for edge.\n");
            exit(EXIT_FAILURE);
        }
        edge->target = v;
        edge->weights = weights;
        edge->period = N;

        graph[u].edges = realloc(graph[u].edges, (graph[u].edge_count + 1) * sizeof(Edge *));
        if (!graph[u].edges) {
            fprintf(stderr, "Error: Memory allocation failed for adjacency list.\n");
            exit(EXIT_FAILURE);
        }
        graph[u].edges[graph[u].edge_count++] = edge;
    }

    fclose(file);
}

// Dijkstra's Algorithm on Time-Expanded Graph
void modified_dijkstra(int start, int end) {
    int total_nodes = V * N;
    int *dist = malloc(total_nodes * sizeof(int));
    bool *visited = malloc(total_nodes * sizeof(bool));

    if (!dist || !visited) {
        fprintf(stderr, "Error: Memory allocation failed for dist/visited arrays.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < total_nodes; i++) {
        dist[i] = INF;
        visited[i] = false;
    }

    PriorityQueue *pq = create_priority_queue(MAX_NODES);
    dist[start * N] = 0;  // Start at (start, time=0)
    push(pq, (State){start, 0, 0});

    while (!is_empty(pq)) {
        State current = pop(pq);
        int u = current.node, time = current.time, cost = current.cost;
        int idx = u * N + time;

        if (visited[idx]) continue;
        visited[idx] = true;

        if (u == end) {
            printf("%d\n", cost);
            free(dist);
            free(visited);
            return;
        }

        for (int i = 0; i < graph[u].edge_count; i++) {
            Edge *edge = graph[u].edges[i];
            int v = edge->target;
            int next_time = (time + 1) % N;
            int weight = edge->weights[time];
            int next_idx = v * N + next_time;

            if (cost + weight < dist[next_idx]) {
                dist[next_idx] = cost + weight;
                push(pq, (State){v, next_time, dist[next_idx]});
            }
        }
    }

    printf("No path found\n");

    free(dist);
    free(visited);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    parse_input(argv[1]);

    int start, end;
    while (scanf("%d %d", &start, &end) != EOF) {
        if (start < 0 || start >= V || end < 0 || end >= V) {
            fprintf(stderr, "Error: Invalid query (%d, %d).\n", start, end);
            continue;
        }
        modified_dijkstra(start, end);
    }

    return 0;
}
