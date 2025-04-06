/*
 * Our graph implementation.
 *
 * Author: A. Tafliovich.
 */

#include "graph.h"

/*********************************************************************
 ** Helper function provided in the starter code
 *********************************************************************/

void printEdge(Edge* edge) {
  if (edge == NULL)
    printf("NULL");
  else
    printf("(%d -- %d, %d)", edge->fromVertex, edge->toVertex, edge->weight);
}

void printEdgeList(EdgeList* head) {
  while (head != NULL) {
    printEdge(head->edge);
    printf(" --> ");
    head = head->next;
  }
  printf("NULL");
}

void printVertex(Vertex* vertex) {
  if (vertex == NULL) {
    printf("NULL");
  } else {
    printf("%d: ", vertex->id);
    printEdgeList(vertex->adjList);
  }
}

void printGraph(Graph* graph) {
  if (graph == NULL) {
    printf("NULL");
    return;
  }
  printf("Number of vertices: %d. Number of edges: %d.\n\n", graph->numVertices,
         graph->numEdges);

  for (int i = 0; i < graph->numVertices; i++) {
    printVertex(graph->vertices[i]);
    printf("\n");
  }
  printf("\n");
}

/*********************************************************************
 ** Required functions
 *********************************************************************/
Edge* newEdge(int fromVertex, int toVertex, int weight) {
  if (weight < 0) {
    fprintf(stderr, "Error: Please use non-negative weight!\n");
    exit(1);
  }

  Edge* edge = malloc(sizeof(Edge));
  if (edge == NULL) {
    perror("Failed to allocate memory for Edge");
    exit(1);
  }

  edge->fromVertex = fromVertex;
  edge->toVertex = toVertex;
  edge->weight = weight;

  return edge;
}

EdgeList* newEdgeList(Edge* edge, EdgeList* next) {
  if (edge == NULL) {
    return NULL;
  }

  EdgeList* node_ptr = malloc(sizeof(EdgeList));
  if (node_ptr == NULL) {
    perror("Failed to allocate memory for EdgeList");
    exit(1);
  }

  node_ptr->edge = edge;
  node_ptr->next = next;

  return node_ptr;
}

Vertex* newVertex(int id, void* value, EdgeList* adjList) {
  Vertex* vertex = malloc(sizeof(Vertex));
  if (vertex == NULL) {
    perror("Failed to allocate memory for Vertex");
    exit(1);
  }

  vertex->id = id;
  vertex->value = value;
  vertex->adjList = adjList;

  return vertex;
}

Graph* newGraph(int numVertices) {
  Graph* graph = malloc(sizeof(Graph));
  if (graph == NULL) {
    perror("Failed to allocate memory for Graph");
    exit(1);
  }

  graph->numVertices = numVertices;
  graph->numEdges = 0;

  graph->vertices = malloc(sizeof(Vertex*) * numVertices);
  if (graph->vertices == NULL) {
    perror("Failed to allocate memory for vertices");
    free(graph);
    exit(1);
  }

  for (int i = 0; i < numVertices; i++) {
    graph->vertices[i] = NULL;
  }

  return graph;
}

void deleteEdgeList(EdgeList* head) {
  EdgeList* current = head;
  while (current != NULL) {
    if (current->edge != NULL) {
      free(current->edge);
    }

    EdgeList* next = current->next;
    free(current);
    current = next;
  }
}

void deleteVertex(Vertex* vertex) {
  if (vertex == NULL) {
    return;
  }

  deleteEdgeList(vertex->adjList);

  free(vertex);
}

void deleteGraph(Graph* graph) {
  if (graph == NULL) {
    return;
  }

  for (int i = 0; i < graph->numVertices; i++) {
    if (graph->vertices[i] != NULL) {
      deleteVertex(graph->vertices[i]);
    }
  }

  free(graph->vertices);

  free(graph);
}