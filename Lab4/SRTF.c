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

void exportGanttChart(int n, int gantt[], int startTimes[], int endTime) {
    printf("Gantt Chart:\n|");

    int currentPID = gantt[0];
    printf(" P%d |", currentPID);

    for (int i = 1; i < n; i++) {
        if (gantt[i] != currentPID) {
            currentPID = gantt[i];
            printf(" P%d |", currentPID);
        }
    }
    printf("\n");

    // Print start times below the Gantt chart
    printf("%d", startTimes[0]);
    currentPID = gantt[0];
    for (int i = 1; i < n; i++) {
        if (gantt[i] != currentPID) {
            printf("    %d", startTimes[i]);
            currentPID = gantt[i];
        }
    }
    printf("   %d\n", endTime);  // Thời điểm kết thúc cuối cùng
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

int findNextProcess(int n, PCB P[], int currentTime) {
    int index = -1;
    for (int i = 0; i < n; i++) {
        if (P[i].iArrival <= currentTime && P[i].iRemaining > 0) {
            if (index == -1 || P[i].iRemaining < P[index].iRemaining) {
                index = i;
            }
        }
    }
    return index;
}

void srtfScheduling(int n, PCB P[]) {
    int completed = 0, currentTime = 0;
    int gantt[100], startTimes[100], ganttIndex = 0;
    
    while (completed < n) {
        int iNext = findNextProcess(n, P, currentTime);
        
        if (iNext == -1) {
            currentTime++;
            continue;
        }
        
        if (P[iNext].iStart == -1) {
            P[iNext].iStart = currentTime;
            P[iNext].iResponse = currentTime - P[iNext].iArrival;
        }

        // Record the Gantt chart data
        if (ganttIndex == 0 || gantt[ganttIndex - 1] != P[iNext].iPID) {
            gantt[ganttIndex] = P[iNext].iPID;
            startTimes[ganttIndex++] = currentTime;
        }

        // Execute the process for one unit time
        P[iNext].iRemaining--;
        currentTime++;

        if (P[iNext].iRemaining == 0) {
            P[iNext].iFinish = currentTime;
            P[iNext].iTaT = P[iNext].iFinish - P[iNext].iArrival;
            P[iNext].iWaiting = P[iNext].iTaT - P[iNext].iBurst;
            completed++;
        }
    }

    // Output Gantt chart with the final end time
    exportGanttChart(ganttIndex, gantt, startTimes, currentTime);
}

int main() {
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    PCB Input[iNumberOfProcess];
    inputProcess(iNumberOfProcess, Input);  // Generates random Arrival and Burst times
    
    printf("\n===== SRTF Scheduling =====\n");
    srtfScheduling(iNumberOfProcess, Input);

    printProcess(iNumberOfProcess, Input);
    calculateAWT(iNumberOfProcess, Input);
    calculateATaT(iNumberOfProcess, Input);

    return 0;
}
