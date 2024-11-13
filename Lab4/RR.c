#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int iPID;
    int iArrival, iBurst, iRemaining;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    srand(time(0));  // Seed for random number generation
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;        // Random Arrival Time [0, 20]
        P[i].iBurst = rand() % 11 + 2;      // Random Burst Time [2, 12]
        P[i].iRemaining = P[i].iBurst;      // Remaining time initially equals burst time
        P[i].iStart = -1;                   // Initialize start time to -1 (not started yet)
        P[i].iFinish = P[i].iWaiting = 0;
        P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("PID\tArrival\tBurst\tStart\tFinish\tResponse\tWaiting\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t\t%d\t%d\n", P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart, P[i].iFinish, P[i].iResponse, P[i].iWaiting, P[i].iTaT);
    }
}

void exportGanttChart(int gantt[], int startTimes[], int endTimes[], int ganttSize) {
    printf("Gantt Chart:\n|");

    for (int i = 0; i < ganttSize; i++) {
        printf(" P%d |", gantt[i]);
    }
    printf("\n");

    // Print start times below the Gantt chart
    printf("%d", startTimes[0]);
    for (int i = 1; i < ganttSize; i++) {
        printf("    %d", startTimes[i]);
    }
    printf("   %d\n", endTimes[ganttSize - 1]);  // Final end time
}

void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++) {
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

void roundRobinScheduling(int n, PCB P[], int quantum) {
    int completed = 0, currentTime = 0;
    int gantt[100], startTimes[100], endTimes[100], ganttIndex = 0;
    int queue[100], front = 0, rear = 0;
    int inQueue[n];
    for (int i = 0; i < n; i++) inQueue[i] = 0;

    // Initial enqueue of processes that have arrived at time 0
    for (int i = 0; i < n; i++) {
        if (P[i].iArrival <= currentTime && P[i].iRemaining > 0) {
            queue[rear++] = i;
            inQueue[i] = 1;
        }
    }

    while (completed < n) {
        if (front == rear) {
            // No process is ready to execute, increment time until one arrives
            currentTime++;
            for (int i = 0; i < n; i++) {
                if (P[i].iArrival <= currentTime && P[i].iRemaining > 0 && inQueue[i] == 0) {
                    queue[rear++] = i;
                    inQueue[i] = 1;
                }
            }
            continue;
        }

        // Dequeue the next process
        int idx = queue[front++];
        if (P[idx].iStart == -1) {
            P[idx].iStart = currentTime;
            P[idx].iResponse = currentTime - P[idx].iArrival;
        }

        // Execute the process for the time slice
        int timeSlice = (P[idx].iRemaining < quantum) ? P[idx].iRemaining : quantum;
        startTimes[ganttIndex] = currentTime;
        gantt[ganttIndex] = P[idx].iPID;
        currentTime += timeSlice;
        endTimes[ganttIndex] = currentTime;
        ganttIndex++;

        P[idx].iRemaining -= timeSlice;

        // Enqueue any newly arrived processes during the time slice
        for (int i = 0; i < n; i++) {
            if (P[i].iArrival <= currentTime && P[i].iRemaining > 0 && inQueue[i] == 0) {
                queue[rear++] = i;
                inQueue[i] = 1;
            }
        }

        // Re-enqueue the process if it still has remaining burst time
        if (P[idx].iRemaining > 0) {
            queue[rear++] = idx;
        } else {
            // Process has finished execution
            P[idx].iFinish = currentTime;
            P[idx].iTaT = P[idx].iFinish - P[idx].iArrival;
            P[idx].iWaiting = P[idx].iTaT - P[idx].iBurst;
            completed++;
        }
    }

    // Output Gantt chart
    exportGanttChart(gantt, startTimes, endTimes, ganttIndex);
}

int main() {
    int iNumberOfProcess, quantum;
    printf("Enter number of processes: ");
    scanf("%d", &iNumberOfProcess);
    printf("Enter quantum time: ");
    scanf("%d", &quantum);

    PCB Input[iNumberOfProcess];
    inputProcess(iNumberOfProcess, Input);  // Generates random Arrival and Burst times

    printf("\n===== Round Robin Scheduling =====\n");
    roundRobinScheduling(iNumberOfProcess, Input, quantum);

    printProcess(iNumberOfProcess, Input);
    calculateAWT(iNumberOfProcess, Input);
    calculateATaT(iNumberOfProcess, Input);

    return 0;
}
