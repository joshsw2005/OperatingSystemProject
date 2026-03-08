#include <iostream>
#include <fstream>
#include <string>

using namespace std;

enum State { NEW, READY, RUNNING, TERMINATED };

struct PCB {
    string pid;
    int arrival, burst, remaining, priority;
    State state;
    PCB* next;
    
    PCB() {
        next = NULL;
        state = NEW;
        arrival = burst = remaining = priority = 0;
    }
};

struct ReadyQueue {
    PCB *head, *tail;
    
    ReadyQueue() {
        head = NULL;
        tail = NULL;
    }

    void enqueue(PCB* p) {
        p->next = NULL;
        if (!tail) head = tail = p;
        else { tail->next = p; tail = p; }
        p->state = READY;
    }
};

void printTrace(ostream& out, int t, PCB* running, ReadyQueue& rq) {
    out << "Time " << t << ":\nRUNNING :\n";
    if (running) {
        out << "PID = " << running->pid << " Arr =" << running->arrival 
            << " Burst =" << running->burst << " Rem =" << running->remaining 
            << " Prio =" << running->priority << " State = RUNNING\n";
    } else {
        out << "IDLE\n";
    }
    out << "READY :\n";
    for (PCB* c = rq.head; c; c = c->next) {
        out << "PID = " << c->pid << " Arr =" << c->arrival << " Rem =" << c->remaining << " State = READY\n";
    }
    out << "-----------------------\n";
}

int main() {
    const int count = 4;
    PCB p[count];
    string pids[] = {"P1", "P2", "P3", "P4"};
    int arr[] = {0, 1, 2, 3}, bur[] = {5, 3, 1, 2}, pri[] = {2, 1, 3, 2};
    
    for(int i=0; i<count; i++) { 
        p[i].pid = pids[i]; 
        p[i].arrival = arr[i]; 
        p[i].burst = p[i].remaining = bur[i]; 
        p[i].priority = pri[i]; 
    }

    ofstream out("output_priority.txt");
    ReadyQueue rq;
    PCB* running = NULL;
    int time = 0, completed = 0;
    string gantt = "|";

    while (completed < count) {
        for (int i = 0; i < count; i++) {
            if (p[i].arrival == time) rq.enqueue(&p[i]);
        }

        if (!running && rq.head) {
            PCB *prev = NULL, *bestPrev = NULL, *best = rq.head;
            PCB *curr = rq.head, *p_curr = NULL;
            
            while (curr) {
                if (curr->priority < best->priority) {
                    best = curr;
                    bestPrev = p_curr;
                }
                p_curr = curr;
                curr = curr->next;
            }
            
            if (!bestPrev) rq.head = best->next; 
            else bestPrev->next = best->next;
            if (best == rq.tail) rq.tail = bestPrev;
            
            running = best;
            running->state = RUNNING;
        }

        printTrace(out, time, running, rq);
        if (running) gantt += " " + running->pid + " |";
        else gantt += " IDLE |";

        if (running) {
            running->remaining--;
            if (running->remaining == 0) {
                completed++;
                running->state = TERMINATED;
                running = NULL;
            }
        }
        time++;
    }

    out << "\nGantt Chart:\n" << gantt << endl;
    out.close();
    
    cout << "Priority simulation complete. Gantt chart generated in output_priority.txt" << endl;
    return 0;
}