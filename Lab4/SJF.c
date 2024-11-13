#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    srand(time(0));  // Seed random number generator
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;  // Random Arrival Time [0, 20]
        P[i].iBurst = rand() % 11 + 2;  // Random Burst Time [2, 12]
        // Initialize values
        P[i].iStart = P[i].iFinish = P[i].iWaiting = 0;
        P[i].iResponse = P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("PID\tArrival\tBurst\tStart\tFinish\tResponse\tWaiting\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t\t%d\t%d\n", P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart, P[i].iFinish, P[i].iResponse, P[i].iWaiting, P[i].iTaT);
    }
}

void exportGanttChart (int n, PCB P[]) {
    printf("Gantt Chart:\n|");
    for (int i = 0; i < n; i++) {
        printf(" P%d |", P[i].iPID);
    }
    printf("\n");
    printf("%d", P[0].iStart);
    for (int i = 0; i < n; i++) {
        printf("  %d", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < (*n) - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int partition (PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        int condition = 0;
        if (iCriteria == SORT_BY_ARRIVAL)
            condition = P[j].iArrival < pivot.iArrival;
        else if (iCriteria == SORT_BY_PID)
            condition = P[j].iPID < pivot.iPID;
        else if (iCriteria == SORT_BY_BURST)
            condition = P[j].iBurst < pivot.iBurst;
        else if (iCriteria == SORT_BY_START)
            condition = P[j].iStart < pivot.iStart;

        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
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
        if (P[i].iArrival <= currentTime) {
            if (index == -1 || P[i].iBurst < P[index].iBurst) {
                index = i;
            }
        }
    }
    return index;
}

int main()
{   
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    PCB Input[iNumberOfProcess + 10];
    PCB TerminatedArray[iNumberOfProcess + 10];
    

    int iRemain = iNumberOfProcess, iTerminated = 0, currentTime = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);


    while (iTerminated < iNumberOfProcess) {
        int iNext = findNextProcess(iRemain, Input, currentTime);
        if (iNext != -1) {
            PCB nextProcess = Input[iNext];
            nextProcess.iStart = currentTime;
            nextProcess.iFinish = nextProcess.iStart + nextProcess.iBurst;
            nextProcess.iResponse = nextProcess.iStart - nextProcess.iArrival;
            nextProcess.iWaiting = nextProcess.iResponse;
            nextProcess.iTaT = nextProcess.iFinish - nextProcess.iArrival;
            currentTime = nextProcess.iFinish;
            pushProcess(&iTerminated, TerminatedArray, nextProcess);
            removeProcess(&iRemain, iNext, Input);
        } else {
            currentTime++;
        }
    }

    printf("\n===== SJF Scheduling =====\n");

    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iNumberOfProcess, Input);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}

