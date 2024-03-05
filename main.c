/*
Nathan Perez 100754066
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>

// Define constraints for resources
#define TOTAL_MEMORY 1024 // in Mbytes
#define MEMORY_FOR_RT 64  // in Mbytes
#define TOTAL_PRINTERS 2    // Total available printers
#define TOTAL_SCANNERS 1    // Total available scanners
#define TOTAL_MODEMS 1      // Total available modems
#define TOTAL_CDS 2         // Total available CD drives
#define MAX_PROCESSES 1000 // Constraint, as stated in lab manual

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

typedef struct {
    int usedMemory;    // Currently used memory in Mbytes
    int usedPrinters;  // Currently used printers
    int usedScanners;  // Currently used scanners
    int usedModems;    // Currently used modems
    int usedCDs;       // Currently used CDs
} ResourceState;

ResourceState resourceState = {0}; // Initialize all resources as not used


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

typedef struct MemoryBlock {
    int startAddress; // For simulation purposes, this could represent the block's starting address
    int size;         // Size of the memory block in Mbytes
    bool isFree;      // Whether the block is free or allocated
    struct MemoryBlock* next; // Pointer to the next block in the list
} MemoryBlock;

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
void findAndProcessNextTask(Queue* rtQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue, Queue* processorQueue);
Process dequeue(Queue* q);
Process* peekFront(Queue* q);
void peekAndPrintFirstEntry(Queue* q);
void printAllProcessesInQueue(Queue* q);
void checkAndRequeueProcesses(Queue* userJobQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue);
bool areResourcesAvailable(Process process);
int queueSize(Queue* q);
void preprocessAndAllocateJobs(Queue* userJobQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue);






































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
    initializeQueue(&rtQueue);
     initializeQueue(&userJobQueue);
    initializeQueue(&highPriorityQueue);
    initializeQueue(&mediumPriorityQueue);
    initializeQueue(&lowPriorityQueue);
    initializeQueue(&processorQueue);

    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, processes, &size);// Call the function to process the file

    // At this point, there is an array of at most 1000 phantum process 

//Tick limit set to 35 for testing
    for (tick = 0; tick < 100 && !endOfTasks; tick++) {
    printf("\n\n%d tick\n ", tick);
    int arrivingCount = countProcessesArrivingAtTick(processes, size, tick);

// If there are tasks that are received, add them to either the real time queue or the user job queue
    if (arrivingCount > 0) {
        printf("%d process(es) arriving at tick %d\n", arrivingCount, tick);   
            // Iterate through all processes to find those arriving at this tick
        for (int i = 0; i < size; i++) {
            if (processes[i].arrivalTime == tick) {
                if (processes[i].priority == 0) {
                    enqueue(&rtQueue, processes[i]);
                    printf("Enqueued RT process at tick %d\n", tick);
                } else if (processes[i].priority != 0) {
                    enqueue(&userJobQueue, processes[i]);
                    printf("Enqueued userJobQueue process at tick %d\n", tick);
                } 
            }
        }                                
        }

// This will allocate resources to the user jobs and assign them to a priority queue
checkAndRequeueProcesses(&userJobQueue, &highPriorityQueue, &mediumPriorityQueue, &lowPriorityQueue);

// After the Queues ( real time & user job queue) have been filled, 

// Check if the processor has a job already
// Decrement its processorTime variable

// Assuming 'processorQueue' is a pointer to a Queue structure
if (!isQueueEmpty(&processorQueue)) {
    Process* currentProcess = peekFront(&processorQueue);

    // Reduce processorTime of the current process
    if (currentProcess->processorTime > 0) {
        currentProcess->processorTime -= 1;
        printf("The processor worked on the problem, reduces processorTime remaining on current task \n");

        // If the current task is completed
        if (currentProcess->processorTime == 0) {
            dequeue(&processorQueue); // Remove the completed task
            // Attempt to find and enqueue the next task from priority queues
            findAndProcessNextTask(&rtQueue, &highPriorityQueue, &mediumPriorityQueue, &lowPriorityQueue, &processorQueue);
        }
    }

    // Preemption logic based on priority
    bool higherPriorityExists = false;
    if (currentProcess->priority == 1 && !isQueueEmpty(&rtQueue)) {
        higherPriorityExists = true;
    } else if (currentProcess->priority == 2 && (!isQueueEmpty(&rtQueue) || !isQueueEmpty(&highPriorityQueue))) {
        higherPriorityExists = true;
    } else if (currentProcess->priority == 3 && (!isQueueEmpty(&rtQueue) || !isQueueEmpty(&highPriorityQueue) || !isQueueEmpty(&mediumPriorityQueue))) {
        higherPriorityExists = true;
    }

    // If a higher priority process exists, preempt the current process
    if (higherPriorityExists) {
        Process preemptedProcess = dequeue(&processorQueue); // Remove current process
        // Enqueue the preempted process to the next lower priority queue
        if (preemptedProcess.priority < 3) {
            preemptedProcess.priority += 1; // Increase priority for requeueing
            switch (preemptedProcess.priority) {
                case 2: enqueue(&mediumPriorityQueue, preemptedProcess); break;
                case 3: enqueue(&lowPriorityQueue, preemptedProcess); break;
            }
        }
        // Enqueue the next highest priority process to the processorQueue
        findAndProcessNextTask(&rtQueue, &highPriorityQueue, &mediumPriorityQueue, &lowPriorityQueue, &processorQueue);
    }

} else {
    printf("The Processor is getting bored.\n");
    printf("Finding Task...\n");
    findAndProcessNextTask(&rtQueue, &highPriorityQueue, &mediumPriorityQueue, &lowPriorityQueue, &processorQueue);
    peekAndPrintFirstEntry(&processorQueue);
}












}
printAllProcessesInQueue(&rtQueue);
printAllProcessesInQueue(&userJobQueue);
printAllProcessesInQueue(&highPriorityQueue);
printAllProcessesInQueue(&mediumPriorityQueue);
printAllProcessesInQueue(&lowPriorityQueue);

    freeQueue(&rtQueue);
    freeQueue(&userJobQueue);
    freeQueue(&highPriorityQueue);
    freeQueue(&mediumPriorityQueue);
    freeQueue(&lowPriorityQueue);
    freeQueue(&processorQueue);
  
    

return 0;
}

// End Of Main
































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
// Modified function to find next task and enqueue it to the processorQueue
void findAndProcessNextTask(Queue* rtQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue, Queue* processorQueue) {
    if (!isQueueEmpty(rtQueue)) {
        Process process = dequeue(rtQueue);
        printf("RT process enqueued to processorQueue with remaining processorTime: %d\n", process.processorTime);
        enqueue(processorQueue, process);
    } else if (!isQueueEmpty(highPriorityQueue)) {
        Process process = dequeue(highPriorityQueue);
        printf("High Priority process enqueued to processorQueue with remaining processorTime: %d\n", process.processorTime);
        enqueue(processorQueue, process);
    } else if (!isQueueEmpty(mediumPriorityQueue)) {
        Process process = dequeue(mediumPriorityQueue);
        printf("Medium Priority process enqueued to processorQueue with remaining processorTime: %d\n", process.processorTime);
        enqueue(processorQueue, process);
    } else if (!isQueueEmpty(lowPriorityQueue)) {
        Process process = dequeue(lowPriorityQueue);
        printf("Low Priority process enqueued to processorQueue with remaining processorTime: %d\n", process.processorTime);
        enqueue(processorQueue, process);
    } else {
        printf("No processes found in any queue. We wait.\n");
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

Process* peekFront(Queue* q) {
    if (q->front == NULL) {
        // Queue is empty, so there's no process at the front to look at.
        return NULL;
    }
    // Return a pointer to the process at the front of the queue.
    return &(q->front->process);
}

void peekAndPrintFirstEntry(Queue* q) {
    Process* frontProcess = peekFront(q); // Assuming peekFront is implemented as shown earlier
    if (frontProcess != NULL) {
        // Print details of the process
        printf("Process at front: Arrival Time: %d, Priority: %d, Processor Time: %d, Memory Requirement: %d\n", 
               frontProcess->arrivalTime, frontProcess->priority, frontProcess->processorTime, frontProcess->memoryRequirement);
        // Add more fields if necessary
    } else {
        // Queue is empty
        printf("The processor is empty. No process to report on.\n");
    }
}

void printAllProcessesInQueue(Queue* q) {
    Node* current = q->front; // Start with the front of the queue
    
    if (current == NULL) {
        printf("The queue is empty.\n");
        return;
    }
    
    printf("Processes in the queue:\n");
    while (current != NULL) {
        // Print details of the process
        printf("Process: Arrival Time: %d, Priority: %d, Processor Time: %d, Memory Requirement: %d\n",
               current->process.arrivalTime, current->process.priority,
               current->process.processorTime, current->process.memoryRequirement);
        // Add more fields if necessary
        
        current = current->next; // Move to the next node in the queue
    }
}

void checkAndRequeueProcesses(Queue* userJobQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue) {
    int size = queueSize(userJobQueue);
    for (int i = 0; i < size; i++) {
        Process process = dequeue(userJobQueue);
        
        preprocessAndAllocateJobs(userJobQueue, highPriorityQueue, mediumPriorityQueue, lowPriorityQueue);
    
        bool resourcesAvailable = areResourcesAvailable(process);
        
        if (resourcesAvailable) {
            // Resources are available, enqueue process based on its priority
            switch (process.priority) {
                case 1:
                    enqueue(highPriorityQueue, process);
                    printf("Moved process to highPriorityQueue with arrival time %d\n", process.arrivalTime);
                    break;
                case 2:
                    enqueue(mediumPriorityQueue, process);
                    printf("Moved process to mediumPriorityQueue with arrival time %d\n", process.arrivalTime);
                    break;
                case 3:
                    enqueue(lowPriorityQueue, process);
                    printf("Moved process to lowPriorityQueue with arrival time %d\n", process.arrivalTime);
                    break;
                default:
                    // Handle unexpected priority
                    printf("Process with unexpected priority %d\n", process.priority);
            }
        } else {
            // Resources are not available, enqueue it back to userJobQueue
            enqueue(userJobQueue, process);
        }
    }
}

bool areResourcesAvailable(Process process) {
    if (resourceState.usedMemory + process.memoryRequirement <= TOTAL_MEMORY - MEMORY_FOR_RT &&
        resourceState.usedPrinters + process.printers <= TOTAL_PRINTERS &&
        resourceState.usedScanners + process.scanners <= TOTAL_SCANNERS &&
        resourceState.usedModems + process.modems <= TOTAL_MODEMS &&
        resourceState.usedCDs + process.cds <= TOTAL_CDS) {
        return true; // All required resources are available
    }
    return false; // Some or all required resources are not available
}

void freeResources(Process process) {
    // Ensure freeing resources doesn't result in negative usage
    resourceState.usedMemory -= process.memoryRequirement;
    resourceState.usedPrinters -= process.printers;
    resourceState.usedScanners -= process.scanners;
    resourceState.usedModems -= process.modems;
    resourceState.usedCDs -= process.cds;

    // Clamp values to 0 to prevent negative resource usage
    resourceState.usedMemory = resourceState.usedMemory < 0 ? 0 : resourceState.usedMemory;
    resourceState.usedPrinters = resourceState.usedPrinters < 0 ? 0 : resourceState.usedPrinters;
    resourceState.usedScanners = resourceState.usedScanners < 0 ? 0 : resourceState.usedScanners;
    resourceState.usedModems = resourceState.usedModems < 0 ? 0 : resourceState.usedModems;
    resourceState.usedCDs = resourceState.usedCDs < 0 ? 0 : resourceState.usedCDs;

    printf("Resources freed for process. Current usage: Memory %d MB, Printers %d, Scanners %d, Modems %d, CDs %d\n",
           resourceState.usedMemory, resourceState.usedPrinters, resourceState.usedScanners,
           resourceState.usedModems, resourceState.usedCDs);
}

int queueSize(Queue* q) {
    int count = 0;
    Node* current = q->front; // Start with the front of the queue
    while (current != NULL) {
        count++; // Increment count for each node in the queue
        current = current->next; // Move to the next node
    }
    return count; // Return the total count of nodes in the queue
}

bool allocateResources(Process process) {
    if (areResourcesAvailable(process)) {
        resourceState.usedMemory += process.memoryRequirement;
        resourceState.usedPrinters += process.printers;
        resourceState.usedScanners += process.scanners;
        resourceState.usedModems += process.modems;
        resourceState.usedCDs += process.cds;
        return true; // Indicate successful resource allocation
    }
    return false; // Resources not available
}

void preprocessAndAllocateJobs(Queue* userJobQueue, Queue* highPriorityQueue, Queue* mediumPriorityQueue, Queue* lowPriorityQueue) {
    Queue tempQueue;
    initializeQueue(&tempQueue); // Assume this initializes a queue
    
    while (!isQueueEmpty(userJobQueue)) {
        Process process = dequeue(userJobQueue);
        
        if (allocateResources(process)) {
            // Depending on priority, move to the correct queue
            switch (process.priority) {
                case 1:
                    enqueue(highPriorityQueue, process);
                    break;
                case 2:
                    enqueue(mediumPriorityQueue, process);
                    break;
                case 3:
                    enqueue(lowPriorityQueue, process);
                    break;
                default:
                    printf("Unexpected priority level: %d\n", process.priority);
            }
        } else {
            // Requeue at the end of userJobQueue if resources are not available
            enqueue(&tempQueue, process);
        }
    }

    // Move all processes from tempQueue back to userJobQueue to maintain original order for those not moved
    while (!isQueueEmpty(&tempQueue)) {
        enqueue(userJobQueue, dequeue(&tempQueue));
    }
}




