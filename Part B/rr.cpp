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
        if (!tail) {
            head = tail = p;
        } else {
            tail->next = p;
            tail = p;
        }
        p->state = READY;
    }

    PCB* dequeue() {
        if (!head) return NULL;
        PCB* temp = head;
        head = head->next;
        if (!head) tail = NULL;
        return temp;
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

    ofstream out("output_rr.txt");
    ReadyQueue rq;
    PCB* running = NULL;
    int time = 0, completed = 0, quantum_timer = 0;
    string gantt = "|";

    while (completed < count) {
        for (int i = 0; i < count; i++) {
            if (p[i].arrival == time) rq.enqueue(&p[i]);
        }

        if (running && quantum_timer == 2) {
            running->state = READY;
            rq.enqueue(running);
            running = NULL;
            quantum_timer = 0;
        }

        if (!running && rq.head) {
            running = rq.dequeue();
            running->state = RUNNING;
            quantum_timer = 0;
        }

        printTrace(out, time, running, rq);
        if (running) gantt += " " + running->pid + " |";
        else gantt += " IDLE |";

        if (running) {
            running->remaining--;
            quantum_timer++;
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
    
    cout << "Round Robin simulation complete. Gantt chart in output_rr.txt" << endl;
    return 0;
}