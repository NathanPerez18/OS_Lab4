/*
Nathan Perez 100754066
End of Day 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>

// Define constraints for resources
#define TOTAL_MEMORY 1024 // in Mbytes
#define MEMORY_FOR_RT 64  // in Mbytes
#define MAX_PROCESSES 1000 // Constraint

// Define a structure for processes
typedef struct {
    int arrivalTime;
    int priority;
    int processorTime;
    int memoryRequirement;
    int printers;
    int scanners;
    int modems;
    int cds;
} Process;

// Define a node in the queue
typedef struct Node {
    Process process;
    struct Node* next;
} Node;

// Define a structure for the queue
typedef struct {
    Node* front; // Points to the front of the queue
    Node* rear;  // Points to the end of the queuet
} Queue;


//Function Declartions
void getFilenameFromUser(char* filename, int max_length);
void processFile(const char* filename);
void insertInOrder(Process array[], Process newProcess, int *size);
void readProcesses(const char* filename, Process processes[], int *size);
int countProcessesArrivingAtTick(Process processes[], int size, int tick);
void enqueue(Queue* q, Process newProcess);
void initializeQueue(Queue* q);
void freeQueue(Queue* q);
bool isQueueEmpty(Queue* q);
void findAndProcessNextTask(Queue* rtQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue);
Process dequeue(Queue* q);

int main() {
    // Variables
    Process processes[MAX_PROCESSES];
    int size;
    int tick;
    bool endOfTasks = false;
    Queue rtQueue;
    Queue userJobQueue;
    Queue highPriorityQueue;
    Queue mediumPriorityQueue;
    Queue lowPriorityQueue;
    Queue processorQueue;
    initializeQueue(&highPriorityQueue);
    initializeQueue(&mediumPriorityQueue);
    initializeQueue(&lowPriorityQueue);
    initializeQueue(&rtQueue);
    initializeQueue(&userJobQueue);
    initializeQueue(&processorQueue);

    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, processes, &size);// Call the function to process the file

    // At this point, there is an array of at most 1000 phantum process 

//Tick limit set to 35 for testing
    for (tick = 0; tick < 35 && !endOfTasks; tick++) {

    printf("%d tick \n\n", tick);
    int arrivingCount = countProcessesArrivingAtTick(processes, size, tick);

    if (arrivingCount > 0) {
        printf("%d process(es) arriving at tick %d\n", arrivingCount, tick);   
            // Iterate through all processes to find those arriving at this tick
           for (int i = 0; i < size; i++) {
    if (processes[i].arrivalTime == tick) {
        if (processes[i].priority == 0) {
            enqueue(&rtQueue, processes[i]);
            printf("Enqueued RT process at tick %d\n", tick);
        } else if (processes[i].priority == 1) {
            enqueue(&highPriorityQueue, processes[i]);
            printf("Enqueued High Priority process at tick %d\n", tick);
        } else if (processes[i].priority == 2) {
            enqueue(&mediumPriorityQueue, processes[i]);
            printf("Enqueued Medium Priority process at tick %d\n", tick);
        } else if (processes[i].priority == 3) {
            enqueue(&lowPriorityQueue, processes[i]);
            printf("Enqueued Low Priority process at tick %d\n", tick);
        }
        }
        }                                
        }


// After the Queues have been filled it is time to see if the cpu is open for work, and if so, where we can find work for it

// Check if the processor has a job already
// Decrement its processorTime variable

if (!isQueueEmpty(&processorQueue)) {
    printf("The processor worked on the problem, reduces processorTime remaining on current task \n");
    // Assuming you have a way to access the process at the front without dequeuing it directly
    if (processorQueue.front->process.processorTime > 0) {
        processorQueue.front->process.processorTime--;
    }
} else {
    printf("The queue is empty.\n");
    findAndProcessNextTask(&rtQueue, &highPriorityQueue, &mediumPriorityQueue, &lowPriorityQueue);
}












}
    freeQueue(&rtQueue);
    freeQueue(&userJobQueue);
    freeQueue(&highPriorityQueue);
    freeQueue(&mediumPriorityQueue);
    freeQueue(&lowPriorityQueue);
    freeQueue(&processorQueue);
  
    

return 0;
}






































// Function to prompt the user for the filename and store it
void getFilenameFromUser(char* filename, int max_length) {
    printf("Please enter name of Dispatcher list: ");// Ask the user for the filename
    fgets(filename, max_length, stdin);
    size_t len = strlen(filename); 
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
}

// Function to process the file
void processFile(const char* filename) {
    FILE *file = fopen(filename, "r"); // Open the file for reading
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    printf("hello there");

    fclose(file); // Close the file when done
}

// Function to read text  file
void readProcesses(const char* filename, Process processes[], int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Process tempProcess;
    *size = 0; // Start with an empty array
    while (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                  &tempProcess.arrivalTime,
                  &tempProcess.priority,
                  &tempProcess.processorTime,
                  &tempProcess.memoryRequirement,
                  &tempProcess.printers,
                  &tempProcess.scanners,
                  &tempProcess.modems,
                  &tempProcess.cds) == 8) {
        insertInOrder(processes, tempProcess, size);
    }

    fclose(file);
}

// Function to insert
void insertInOrder(Process array[], Process newProcess, int *size) {
    int i = *size - 1;
    // Find the correct position for the new process
    while (i >= 0 && array[i].arrivalTime > newProcess.arrivalTime) {
        array[i + 1] = array[i]; // Shift elements to the right
        i--;
    }
    array[i + 1] = newProcess; // Insert the new process
    (*size)++; // Increase the size of the array
}

// Function to count how many processes have an arrivalTime equal to the tick
int countProcessesArrivingAtTick(Process processes[], int size, int tick) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (processes[i].arrivalTime == tick) {
            count++;
        }
    }
    return count;
}

void initializeQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

void enqueue(Queue* q, Process newProcess) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Unable to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->process = newProcess;
    newNode->next = NULL;
    
    if (q->rear == NULL) { // If the queue is empty
        q->front = q->rear = newNode; // The new node is both the front and rear
    } else { // If the queue is not empty
        q->rear->next = newNode; // Link the new node to the end of the queue
        q->rear = newNode; // Update the rear to the new node
    }
}

void freeQueue(Queue* q) {
    Node* current = q->front; // Start with the front of the queue
    Node* next;

    while (current != NULL) {
        next = current->next; // Save the next node
        free(current); // Free the current node
        current = next; // Move to the next node
    }

    // After freeing all nodes, reset the front and rear pointers
    q->front = NULL;
    q->rear = NULL;
}
bool isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

// Function to find next task
void findAndProcessNextTask(Queue* rtQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue) {
    if (!isQueueEmpty(rtQueue)) {
        // Found a process in the real-time queue
        Process process = dequeue(rtQueue);
        printf("Processing RT process with remaining processorTime: %d\n", process.processorTime);
    } else if (!isQueueEmpty(highPriorityQueue)) {
        // Found a process in the high-priority queue
        Process process = dequeue(highPriorityQueue);
        printf("Processing High Priority process with remaining processorTime: %d\n", process.processorTime);
    } else if (!isQueueEmpty(mediumPriorityQueue)) {
        // Found a process in the medium-priority queue
        Process process = dequeue(mediumPriorityQueue);
        printf("Processing Medium Priority process with remaining processorTime: %d\n", process.processorTime);
    } else if (!isQueueEmpty(lowPriorityQueue)) {
        // Found a process in the low-priority queue
        Process process = dequeue(lowPriorityQueue);
        printf("Processing Low Priority process with remaining processorTime: %d\n", process.processorTime);
    } else {
        printf("No processes found in any queue. We Wait.\n");
    }
}

// Function for de queue
Process dequeue(Queue* q) {
    if (q->front == NULL) {
        fprintf(stderr, "Attempt to dequeue from an empty queue\n");
        exit(EXIT_FAILURE); // Or handle error as appropriate
    }
    
    Node* temp = q->front;
    Process process = temp->process; // Copy the process data
    q->front = q->front->next;
    
    if (q->front == NULL) { // If the queue becomes empty after dequeuing
        q->rear = NULL;
    }
    
    free(temp); // Free the memory allocated for the dequeued node
    return process; // Return the dequeued process
}

