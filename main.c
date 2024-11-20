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
    fscanf(file, "%d %d", &V, &N);
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
            fscanf(file, "%d", &weights[i]);
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
    PriorityQueue *pq = create_priority_queue(V * N); // Create priority queue
    int dist[V][N]; // Distance matrix for (node, step)
    bool visited[V][N]; // Visited states (node, step)
    int prev[V]; // To reconstruct path

    // Initialize distances and visited states
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < N; j++) {
            dist[i][j] = INF;
            visited[i][j] = false;
        }
        prev[i] = -1;
    }

    // Start with the initial node at step 0
    dist[start][0] = 0;
    push(pq, (State){start, 0, 0});

    while (!is_empty(pq)) {
        State current = pop(pq);
        int u = current.node, cost = current.cost, step = current.step;

        // If already visited this state, skip
        if (visited[u][step]) continue;
        visited[u][step] = true;

        // If we've reached the destination, terminate early
        if (u == end) {
            printf("Shortest path cost: %d\n", cost);
            // Reconstruct and print the path here
            return;
        }

        // Relax all neighbors
        for (int i = 0; i < graph[u].edge_count; i++) {
            Edge *edge = graph[u].edges[i];
            int v = edge->target;
            int weight = dynamic_weight(edge, step);
            int next_step = (step + 1) % N;

            // Relax the edge
            if (cost + weight < dist[v][next_step]) {
                dist[v][next_step] = cost + weight;
                prev[v] = u; // Track the predecessor for path reconstruction
                push(pq, (State){v, dist[v][next_step], next_step});
            }
        }
    }

    printf("No path found.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    parse_input(argv[1]);

    int start, end;
    printf("Enter queries (start end):\n");
    while (scanf("%d %d", &start, &end) != EOF) {
        modified_dijkstra(start, end);
    }

    return 0;
}
