/*
 * Our min-heap implementation.
 *
 * Author (starter code): A. Tafliovich.
 */

#include "minheap.h"

#define ROOT_INDEX 1
#define NOTHING -1

typedef struct min_node {
  int priority;   // the smaller priority of two child node
  int nodeIndex;  // the nodeIndex of the node with smaller priority
} MinNode;

/*************************************************************************
 ** Suggested helper functions -- to help designing your code
 *************************************************************************/
/* Returns node at index 'nodeIndex' in minheap 'heap'.
 * Precondition: 'nodeIndex' is a valid index in 'heap'
 *               'heap' is non-empty
 */
HeapNode nodeAt(MinHeap* heap, int nodeIndex) { return heap->arr[nodeIndex]; }

/* Returns priority of node at index 'nodeIndex' in minheap 'heap'.
 * Precondition: 'nodeIndex' is a valid index in 'heap'
 *               'heap' is non-empty
 */
int priorityAt(MinHeap* heap, int nodeIndex) {
  return heap->arr[nodeIndex].priority;
}

/* Returns ID of node at index 'nodeIndex' in minheap 'heap'.
 * Precondition: 'nodeIndex' is a valid index in 'heap'
 *               'heap' is non-empty
 */
int idAt(MinHeap* heap, int nodeIndex) { return heap->arr[nodeIndex].id; }

/* Returns index of node with ID 'id' in minheap 'heap'.
 * Precondition: 'id' is a valid ID in 'heap'
 *               'heap' is non-empty
 */
int indexOf(MinHeap* heap, int id) { return heap->indexMap[id]; }

/* Returns True if 'maybeIdx' is a valid index in minheap 'heap', and 'heap'
 * stores an element at that index. Returns False otherwise.
 */
bool isValidIndex(MinHeap* heap, int maybeIdx) {
  if (heap == NULL) {
    return false;
  }
  if (maybeIdx < 1 || maybeIdx > heap->size) {
    return false;
  }
  int maybeId = idAt(heap, maybeIdx);
  if (maybeId < 0 || maybeId >= heap->capacity) {
    return false;
  }
  if (indexOf(heap, maybeId) != maybeIdx) {
    return false;
  }
  return true;
}

/* Returns the index of the left child of a node at index 'nodeIndex' in
 * minheap 'heap', if such exists.  Returns NOTHING if there is no such left
 * child.
 */
int leftIdx(MinHeap* heap, int nodeIndex) {
  if (!isValidIndex(heap, nodeIndex)) {
    fprintf(stderr, "Invalid nodeIndex or heap does not exist\n");
    exit(1);
  }
  int left_child = 2 * nodeIndex;
  if (left_child > heap->size) {
    return NOTHING;
  }
  return left_child;
}

/* Returns the index of the right child of a node at index 'nodeIndex' in
 * minheap 'heap', if such exists.  Returns NOTHING if there is no such right
 * child.
 */
int rightIdx(MinHeap* heap, int nodeIndex) {
  if (!isValidIndex(heap, nodeIndex)) {
    fprintf(stderr, "Invalid nodeIndex or heap does not exist\n");
    exit(1);
  }
  int right_child = 2 * nodeIndex + 1;
  if (right_child > heap->size) {
    return NOTHING;
  }
  return right_child;
}

/* Returns the index of the parent of a node at index 'nodeIndex' in minheap
 * 'heap', if such exists.  Returns NOTHING if there is no such parent.
 */
int parentIdx(MinHeap* heap, int nodeIndex) {
  if (!isValidIndex(heap, nodeIndex)) {
    fprintf(stderr, "Invalid nodeIndex or heap does not exist\n");
    exit(1);
  }
  if (nodeIndex == ROOT_INDEX) {
    return NOTHING;
  }
  return nodeIndex / 2;
}

/* Swaps contents of heap->arr[index1] and heap->arr[index2] if both 'index1'
 * and 'index2' are valid indices for minheap 'heap'. Has no effect
 * otherwise.
 */
void swap(MinHeap* heap, int index1, int index2) {
  if (!isValidIndex(heap, index1) || !isValidIndex(heap, index2)) {
    return;
  }
  HeapNode temp_container = nodeAt(heap, index1);
  heap->arr[index1] = nodeAt(heap, index2);
  heap->arr[index2] = temp_container;
  heap->indexMap[idAt(heap, index1)] = index1;
  heap->indexMap[idAt(heap, index2)] = index2;
}

/* Bubbles up the element newly inserted into minheap 'heap' at index
 * 'nodeIndex', if 'nodeIndex' is a valid index for heap. Has no effect
 * otherwise.
 */
void bubbleUp(MinHeap* heap, int nodeIndex) {
  if (!isValidIndex(heap, nodeIndex)) {
    return;
  }
  int parentIndex = parentIdx(heap, nodeIndex);
  while (parentIndex != NOTHING && isValidIndex(heap, nodeIndex) &&
         priorityAt(heap, nodeIndex) < priorityAt(heap, parentIndex)) {
    swap(heap, nodeIndex, parentIndex);
    nodeIndex = parentIndex;
    parentIndex = parentIdx(heap, nodeIndex);
  }
}

/*Find the node with smaller priority between the two child nodes of
 *the node at index 'nodeIndex' in minheap 'heap'.
 *If no child, then use the its priority and index.
 */
MinNode find_min(MinHeap* heap, int nodeIndex) {
  if (!isValidIndex(heap, nodeIndex)) {
    fprintf(stderr, "Invalid nodeIndex or the heap is not exist\n");
    exit(1);
  }
  int left_child = leftIdx(heap, nodeIndex);
  int right_child = rightIdx(heap, nodeIndex);
  MinNode result;
  if (left_child == NOTHING) {
    result.priority = priorityAt(heap, nodeIndex);
    result.nodeIndex = nodeIndex;
    return result;
  }
  if (right_child == NOTHING) {
    result.priority = priorityAt(heap, left_child);
    result.nodeIndex = left_child;
    return result;
  }
  if (priorityAt(heap, left_child) < priorityAt(heap, right_child)) {
    result.priority = priorityAt(heap, left_child);
    result.nodeIndex = left_child;
    return result;
  }
  result.priority = priorityAt(heap, right_child);
  result.nodeIndex = right_child;
  return result;
}

/* Bubbles down the element newly inserted into minheap 'heap' at the root,
 * if it exists. Has no effect otherwise.
 */
void bubbleDown(MinHeap* heap) {
  if (heap == NULL || heap->size < 1) {
    return;
  }
  int nodeIndex = ROOT_INDEX;
  MinNode min_node_info = find_min(heap, nodeIndex);
  while (isValidIndex(heap, nodeIndex) &&
         priorityAt(heap, nodeIndex) > min_node_info.priority) {
    swap(heap, nodeIndex, min_node_info.nodeIndex);
    nodeIndex = min_node_info.nodeIndex;
    min_node_info = find_min(heap, nodeIndex);
  }
}
/*********************************************************************
 * Required functions
 ********************************************************************/
HeapNode getMin(MinHeap* heap) { return nodeAt(heap, ROOT_INDEX); }

HeapNode extractMin(MinHeap* heap) {
  HeapNode minNode = getMin(heap);
  heap->arr[1] = nodeAt(heap, heap->size);
  heap->indexMap[idAt(heap, ROOT_INDEX)] = ROOT_INDEX;
  heap->size -= 1;
  bubbleDown(heap);
  return minNode;
}

void insert(MinHeap* heap, int priority, int id) {
  heap->size += 1;
  int nodeIndex = heap->size;
  heap->arr[nodeIndex].priority = priority;
  heap->arr[nodeIndex].id = id;
  heap->indexMap[id] = nodeIndex;
  bubbleUp(heap, nodeIndex);
}

int getPriority(MinHeap* heap, int id) {
  int index = indexOf(heap, id);
  return priorityAt(heap, index);
}

bool decreasePriority(MinHeap* heap, int id, int newPriority) {
  if (heap == NULL || id < 0 || id >= heap->capacity) {
    fprintf(stderr, "Invalid id!\n");
    return false;
  }
  int index = indexOf(heap, id);
  if (!isValidIndex(heap, index)) {
    fprintf(stderr, "Invalid id index!\n");
    return false;
  }
  int oldPriority = getPriority(heap, id);
  if (oldPriority <= newPriority) {
    return false;
  }
  heap->arr[index].priority = newPriority;
  bubbleUp(heap, index);
  return true;
}

MinHeap* newHeap(int capacity) {
  MinHeap* heap = malloc(sizeof(MinHeap));
  if (!heap) {
    fprintf(stderr, "Memory allocation failed for MinHeap\n");
    exit(1);
  }
  heap->size = 0;
  heap->capacity = capacity;
  heap->arr = malloc((capacity + 1) * sizeof(HeapNode));
  if (!heap->arr) {
    fprintf(stderr, "Memory allocation failed for the heaparray!\n");
    free(heap);
    exit(1);
  }
  heap->indexMap = malloc(capacity * sizeof(int));
  if (!heap->indexMap) {
    fprintf(stderr, "Memory allocation failed for the index array\n");
    free(heap->arr);
    free(heap);
    exit(1);
  }
  for (int i = 0; i < capacity; ++i) {
    heap->indexMap[i] = NOTHING;
  }
  return heap;
}

void deleteHeap(MinHeap* heap) {
  if (heap == NULL) {
    return;
  }
  free(heap->arr);
  free(heap->indexMap);
  free(heap);
}
/*********************************************************************
 ** Helper function provided
 *********************************************************************/
void printHeap(MinHeap* heap) {
  printf("MinHeap with size: %d\n\tcapacity: %d\n\n", heap->size,
         heap->capacity);
  printf("index: priority [ID]\t ID: index\n");
  for (int i = 0; i < heap->capacity; i++)
    printf("%d: %d [%d]\t\t%d: %d\n", i, priorityAt(heap, i), idAt(heap, i), i,
           indexOf(heap, i));
  printf("%d: %d [%d]\t\t\n", heap->capacity, priorityAt(heap, heap->capacity),
         idAt(heap, heap->capacity));
  printf("\n\n");
}
