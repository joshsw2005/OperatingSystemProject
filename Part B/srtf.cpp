#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

enum State { NEW, READY, RUNNING, TERMINATED };

struct PCB {
    string pid;
    int arrival;
    int burst;
    int remaining;
    int priority;
    State state;
    int start_time;
    int finish_time;
    bool started;
    PCB* next;

    PCB() {
        next = NULL;
        state = NEW;
        start_time = -1;
        started = false;
        arrival = burst = remaining = priority = 0;
    }
};

struct ReadyQueue {
    PCB* head;
    PCB* tail;

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

    bool isEmpty() { return head == NULL; }
};

void printTrace(ostream& out, int t, PCB* running, ReadyQueue& rq) {
    out << "Time " << t << ":" << endl;
    out << "RUNNING :" << endl;
    if (running) {
        out << "PID = " << running->pid << " Arr =" << running->arrival 
            << " Burst =" << running->burst << " Rem =" << running->remaining 
            << " Prio =" << running->priority << " State = RUNNING" << endl;
    } else {
        out << "IDLE" << endl;
    }

    out << "READY :" << endl;
    PCB* curr = rq.head;
    while (curr) {
        out << "PID = " << curr->pid << " Arr =" << curr->arrival 
            << " Burst =" << curr->burst << " Rem =" << curr->remaining 
            << " Prio =" << curr->priority << " State = READY" << endl;
        curr = curr->next;
    }
    out << "-----------------------" << endl;
}

void simulateSRTF(PCB* processes, int count, const string& filename) {
    ofstream out(filename.c_str());
    int time = 0;
    int completed = 0;
    PCB* running = NULL;
    ReadyQueue rq;
    string gantt = "|";

    while (completed < count) {
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival == time) {
                rq.enqueue(&processes[i]);
            }
        }

        PCB* shortest = NULL;
        PCB* curr = rq.head;
        while (curr) {
            if (!shortest || curr->remaining < shortest->remaining) {
                shortest = curr;
            }
            curr = curr->next;
        }

        if (running) {
            if (shortest && shortest->remaining < running->remaining) {
                running->state = READY;
                rq.enqueue(running); 
                running = NULL; 
            }
        }

        if (!running && !rq.isEmpty()) {
            PCB* prev = NULL;
            PCB* bestPrev = NULL;
            PCB* best = rq.head;
            curr = rq.head;
            while(curr) {
                if(curr->remaining < best->remaining) {
                    best = curr;
                    bestPrev = prev;
                }
                prev = curr;
                curr = curr->next;
            }
            
            if(!bestPrev) rq.head = best->next;
            else bestPrev->next = best->next;
            if(best == rq.tail) rq.tail = bestPrev;
            
            running = best;
            running->state = RUNNING;
            if (!running->started) {
                running->start_time = time;
                running->started = true;
            }
        }

        printTrace(out, time, running, rq);
        if (running) gantt += " " + running->pid + " |";
        else gantt += " IDLE |";

        if (running) {
            running->remaining--;
            if (running->remaining == 0) {
                running->state = TERMINATED;
                running->finish_time = time + 1;
                completed++;
                running = NULL;
            }
        }
        time++;
    }
    out << "\nGantt Chart: " << gantt << endl;
    out.close();
}

int main() {
    const int count = 4;
    PCB processes[count];
    
    processes[0].pid = "P1"; processes[0].arrival = 0; processes[0].burst = 5; 
    processes[0].remaining = 5; processes[0].priority = 2;

    processes[1].pid = "P2"; processes[1].arrival = 1; processes[1].burst = 3; 
    processes[1].remaining = 3; processes[1].priority = 1;

    processes[2].pid = "P3"; processes[2].arrival = 2; processes[2].burst = 1; 
    processes[2].remaining = 1; processes[2].priority = 3;

    processes[3].pid = "P4"; processes[3].arrival = 3; processes[3].burst = 2; 
    processes[3].remaining = 2; processes[3].priority = 2;

    simulateSRTF(processes, count, "output_srtf.txt");

    cout << "SRTF simulation complete." << endl;

    return 0;
}