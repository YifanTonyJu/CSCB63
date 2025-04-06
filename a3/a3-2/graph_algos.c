/*
 * Graph algorithms.
 *
 * Author (of starter code): A. Tafliovich.
 */

#include <limits.h>

#include "graph.h"
#include "minheap.h"

#define NOTHING -1
#define DEBUG 0

typedef struct records {
  int numVertices;    // total number of vertices in the graph
                      // vertex IDs are 0, 1, ..., numVertices-1
  MinHeap* heap;      // priority queue
  bool* finished;     // finished[id] is true iff vertex id is finished
                      //   i.e. no longer in the PQ
  int* predecessors;  // predecessors[id] is the predecessor of vertex id
  Edge* tree;         // keeps edges for the resulting tree
  int numTreeEdges;   // current number of edges in mst
} Records;

/*************************************************************************
 ** Suggested helper functions -- part of starter code
 *************************************************************************/
/* Returns true iff 'heap' is NULL or is empty. */
bool isEmpty(MinHeap* heap) { return heap == NULL || heap->size == 0; }

/* Creates, populates, and returns a MinHeap to be used by Prim's and
 * Dijkstra's algorithms on Graph 'graph' starting from vertex with ID
 * 'startVertex'.
 * Precondition: 'startVertex' is valid in 'graph'
 */
MinHeap* initHeap(Graph* graph, int startVertex) {
  if (graph == NULL) {
    return NULL;
  }

  int ver_num = graph->numVertices;
  MinHeap* heap = newHeap(ver_num);
  if (heap == NULL) {
    perror("Failed to allocate MinHeap");
    exit(1);
  }

  for (int i = 0; i < ver_num; i++) {
    if (i == startVertex) {
      insert(heap, 0, i);
    } else {
      insert(heap, INT_MAX, i);
    }
  }

  return heap;
}

/* Creates, populates, and returns all records needed to run Prim's and
 * Dijkstra's algorithms on Graph 'graph' starting from vertex with ID
 * 'startVertex'.
 * Precondition: 'startVertex' is valid in 'graph'
 */
Records* initRecords(Graph* graph, int startVertex) {
  if (graph == NULL) {
    return NULL;
  }

  Records* records = malloc(sizeof(Records));
  if (records == NULL) {
    perror("Failed to allocate Records");
    exit(1);
  }

  int ver_num = graph->numVertices;
  records->numVertices = ver_num;
  records->numTreeEdges = 0;

  records->heap = initHeap(graph, startVertex);
  if (isEmpty(records->heap)) {
    free(records);
    exit(1);
  }

  records->finished = malloc(sizeof(bool) * ver_num);
  if (records->finished == NULL) {
    perror("Failed to allocate finished array");
    deleteHeap(records->heap);
    free(records);
    exit(1);
  }
  for (int i = 0; i < ver_num; i++) {
    records->finished[i] = false;
  }

  records->predecessors = malloc(sizeof(int) * ver_num);
  if (records->predecessors == NULL) {
    perror("Failed to allocate predecessors array");
    free(records->finished);
    deleteHeap(records->heap);
    free(records);
    exit(1);
  }
  for (int i = 0; i < ver_num; i++) {
    records->predecessors[i] = NOTHING;
  }

  records->tree = malloc(sizeof(Edge) * (ver_num));
  if (records->tree == NULL) {
    perror("Failed to allocate tree array");
    free(records->predecessors);
    free(records->finished);
    deleteHeap(records->heap);
    free(records);
    exit(1);
  }

  return records;
}

/* Prints the status of all current algorithm data: good for debugging. */
void printRecords(Records* records);

/* Add a new edge to records at index ind. */
void addTreeEdge(Records* records, int ind, int fromVertex, int toVertex,
                 int weight) {
  if (records == NULL || ind < 0 || ind > records->numVertices - 1) {
    return;
  }

  Edge* edge = &(records->tree[ind]);
  edge->fromVertex = fromVertex;
  edge->toVertex = toVertex;
  edge->weight = weight;
}

/* Creates and returns a path from 'vertex' to 'startVertex' from edges
 * in the distance tree 'distTree'.
 */
EdgeList* makePath(Edge* distTree, int numVertices, int vertex,
                   int startVertex) {
  if (vertex < 0 || vertex >= numVertices || startVertex < 0 ||
      startVertex >= numVertices) {
    fprintf(stderr, "Error: Please use correct 'vertex' and 'startVertex'!\n");
    exit(1);
  }
  if (distTree == NULL || vertex == startVertex) {
    return NULL;
  }

  EdgeList* path_head = NULL;
  EdgeList* current = NULL;
  int current_vertex = vertex;

  while (current_vertex != startVertex) {
    bool found = false;

    int distTree_index = 0;

    while (distTree_index >= 0 && distTree_index < numVertices) {
      Edge current_distTree_edge = distTree[distTree_index];
      int pre_weight = 0;
      if (current_distTree_edge.fromVertex == current_vertex) {
        for (int i = 0; i < numVertices; ++i) {
          if (distTree[i].fromVertex == current_distTree_edge.toVertex) {
            pre_weight = distTree[i].weight;
            break;
          }
        }
        Edge* edge = newEdge(current_distTree_edge.fromVertex,
                             current_distTree_edge.toVertex,
                             current_distTree_edge.weight - pre_weight);
        if (edge == NULL) {
          deleteEdgeList(path_head);
          exit(1);
        }

        EdgeList* new_node = newEdgeList(edge, NULL);
        if (new_node == NULL) {
          free(edge);
          deleteEdgeList(path_head);
          exit(1);
        }
        if (path_head == NULL) {
          path_head = new_node;
          current = path_head;
        } else {
          current->next = new_node;
          current = current->next;
        }
        current_vertex = current_distTree_edge.toVertex;
        found = true;
        break;
      }
      distTree_index += 1;
    }

    if (!found) {
      deleteEdgeList(path_head);
      return NULL;
    }
  }
  return path_head;
}
/*************************************************************************
 ** Required functions
 *************************************************************************/
Edge* getMSTprim(Graph* graph, int startVertex) {
  if (graph == NULL || startVertex < 0 || startVertex >= graph->numVertices) {
    return NULL;
  }

  Records* records = initRecords(graph, startVertex);
  if (records == NULL) {
    fprintf(stderr, "Error: Fail to initial a records!\n");
    exit(1);
  }

  while (!isEmpty(records->heap)) {
    HeapNode u = extractMin(records->heap);
    int uid = u.id;
    records->finished[uid] = true;

    int pred = records->predecessors[uid];

    if (pred != NOTHING) {
      addTreeEdge(records, records->numTreeEdges, uid, pred, u.priority);
      records->numTreeEdges += 1;
    }

    Vertex* u_vertex = graph->vertices[uid];
    EdgeList* u_adj = u_vertex->adjList;

    while (u_adj != NULL) {
      int vid = u_adj->edge->toVertex;
      int weight_uv = u_adj->edge->weight;

      if (!records->finished[vid] &&
          weight_uv < getPriority(records->heap, vid)) {
        decreasePriority(records->heap, vid, weight_uv);
        records->predecessors[vid] = uid;
      }

      u_adj = u_adj->next;
    }
  }

  Edge* mst = records->tree;

  free(records->finished);
  free(records->predecessors);
  deleteHeap(records->heap);
  free(records);

  return mst;
}

Edge* getDistanceTreeDijkstra(Graph* graph, int startVertex) {
  if (graph == NULL || startVertex < 0 || startVertex >= graph->numVertices) {
    return NULL;
  }

  Records* records = initRecords(graph, startVertex);
  if (records == NULL) {
    fprintf(stderr, "Error: Failed to initialize records!\n");
    exit(1);
  }

  while (!isEmpty(records->heap)) {
    HeapNode u = extractMin(records->heap);
    int uid = u.id;
    int u_dist = u.priority;
    records->finished[uid] = true;

    if (uid == startVertex) {
      addTreeEdge(records, uid, uid, uid, 0);
    } else {
      int pred = records->predecessors[uid];
      addTreeEdge(records, uid, uid, pred, u_dist);
    }

    EdgeList* u_adj = graph->vertices[uid]->adjList;
    while (u_adj != NULL) {
      int vid = u_adj->edge->toVertex;
      int weight_uv = u_adj->edge->weight;
      int new_dist = u_dist + weight_uv;

      if (!records->finished[vid] &&
          new_dist < getPriority(records->heap, vid)) {
        decreasePriority(records->heap, vid, new_dist);
        records->predecessors[vid] = uid;
      }

      u_adj = u_adj->next;
    }
  }

  Edge* result = records->tree;

  free(records->finished);
  free(records->predecessors);
  deleteHeap(records->heap);
  free(records);

  return result;
}

EdgeList** getShortestPaths(Edge* distTree, int numVertices, int startVertex) {
  if (distTree == NULL || startVertex < 0 || startVertex >= numVertices) {
    return NULL;
  }

  EdgeList** paths = malloc(sizeof(EdgeList*) * numVertices);
  if (paths == NULL) {
    perror("Failed to allocate EdgeList pointer array");
    exit(1);
  }

  for (int vertexId = 0; vertexId < numVertices; vertexId++) {
    if (vertexId == startVertex) {
      paths[vertexId] = NULL;
    } else {
      paths[vertexId] = makePath(distTree, numVertices, vertexId, startVertex);
    }
  }

  return paths;
}

/*************************************************************************
 ** Provided helper functions -- part of starter code to help you debug!
 *************************************************************************/
void printRecords(Records* records) {
  if (records == NULL) return;

  int numVertices = records->numVertices;
  printf("Reporting on algorithm's records on %d vertices...\n", numVertices);

  printf("The PQ is:\n");
  printHeap(records->heap);

  printf("The finished array is:\n");
  for (int i = 0; i < numVertices; i++)
    printf("\t%d: %d\n", i, records->finished[i]);

  printf("The predecessors array is:\n");
  for (int i = 0; i < numVertices; i++)
    printf("\t%d: %d\n", i, records->predecessors[i]);

  printf("The TREE edges are:\n");
  for (int i = 0; i < records->numTreeEdges; i++) printEdge(&records->tree[i]);

  printf("... done.\n");
}
