#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <map>
#include <numeric>
#include <windows.h>
#include <iomanip>

struct process_info {
    char processID{};
    int burst_time{};
    int arrival_time{};
    int start_time{};
    int wait_time{};
    int turnaround_time{};
    int quantum_remaining{};
    process_info()= default;
    process_info(char pid, int bt, int at) : processID{pid}, burst_time{bt}, arrival_time{at}, start_time{-1}, wait_time{0}, turnaround_time{0}{};
};

void read_data_into_vector(std::vector<process_info>&);
void shortest_job_first(std::vector<process_info>);
void round_robin(std::vector<process_info>, int);
void shortest_remaining_time_first(std::vector<process_info>);


int main() {

    std::vector<process_info> processVector;
    read_data_into_vector(processVector);
    shortest_job_first(processVector);
    round_robin(processVector, 4);
    shortest_remaining_time_first(processVector);

    /*auto compare = [](int lhs, int rhs) {
        return lhs > rhs;
    };

    std::vector<int> v{1, 5, 4};
    std::make_heap(v.begin(), v.end(), compare);
    for(int i = 0; i < 5; i++) {
        std::for_each(v.begin(), v.end(),
        [](int i){std::cout << i << " ";});
        std::cout << std::endl;
        std::pop_heap(v.begin(), v.end(), compare);
    }*/

    return 0;
}



void read_data_into_vector(std::vector<process_info>& v) {
    std::ifstream inputFile("process_data.txt");
    char id;
    int bt, at;
    if(inputFile) {
        while(!inputFile.eof()) {
            inputFile >> id >> bt >> at;
            v.emplace_back(process_info{id, bt, at});
        }
    }
}


void shortest_job_first(std::vector<process_info> processVector) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),11);
    std::cout << "SJF Scheduling" << std::endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
    std::map<char, std::pair<int, int>> process_stats;
    auto compare = [](const process_info& lhs, const process_info& rhs) {
        return lhs.burst_time > rhs.burst_time;
    };

    process_info curr_process_info{};
    bool process_executing{false};
    std::sort(processVector.begin(), processVector.end(),
              [](const process_info& pi1, const process_info& pi2) {
                  return pi1.arrival_time < pi2.arrival_time;
              });
    std::priority_queue<process_info, std::vector<process_info>, decltype(compare)> priorityQueue(compare);
    int current_time{0};
    while(true) {
        for(auto it = processVector.begin(); it != processVector.end(); it++) {
            if(it->arrival_time == current_time) {
                priorityQueue.push(*it);
                processVector.erase(it, it + 1);
                it--;
            } else break;
        }
        if(process_executing) {
            if(curr_process_info.start_time + curr_process_info.burst_time == current_time)
                process_executing = false;
        }
        if(!priorityQueue.empty() && !process_executing) {
            std::cout << current_time << " " << priorityQueue.top().processID << "\tProcess terminated" << std::endl;
            curr_process_info = priorityQueue.top();
            priorityQueue.pop();
            process_executing = true;
            curr_process_info.start_time = current_time;
            int wait_time = curr_process_info.start_time - curr_process_info.arrival_time;
            int turnaround_time = wait_time + curr_process_info.burst_time;
            process_stats[curr_process_info.processID] = std::make_pair(wait_time, turnaround_time);
        }
        if(!process_executing && processVector.empty()) {
            std::cout << current_time << "\tComplete\n" << std::endl;
            auto total_wait_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                   [](int s, const std::pair<char, std::pair<int, int>>& p) {return s + p.second.first;});
            auto total_turnaround_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                         [](int s, const std::pair<char, std::pair<int, int>>& p) {return s + p.second.second;});

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),11);
            std::cout << std::left << std::setw(13) << "Process ID" << std::setw(18) << "Turnaround Time" << "Waiting Time" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
            for(const auto& elem : process_stats) {
                std::cout << std::left << std::setw(13) << elem.first << std::setw(18) << elem.second.second << elem.second.first << std::endl;
            }
            std::cout << std::left << std::setw(13) << "Average" << std::setw(18) << (float) total_turnaround_time / process_stats.size();
            std::cout << (float) total_wait_time / process_stats.size() << std::endl << std::endl << std::endl;
            break;
        }
        current_time++;
    }
}


void round_robin(std::vector<process_info> processVector, int quantum = 4) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),12);
    std::cout << "Round Robin Scheduling" << std::endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
    std::map<char, std::pair<int, int>> process_stats;
    process_info curr_process_info{};
    bool process_executing{false};
    std::vector<process_info> processQueue;
    int current_time{0};
    std::sort(processVector.begin(), processVector.end(),
              [](const process_info& pi1, const process_info& pi2) {
                  return pi1.arrival_time < pi2.arrival_time;
              });
    while(true) {
        for(auto it = processVector.begin(); it != processVector.end(); it++) {
            if(it->arrival_time == current_time) {
                processQueue.push_back(*it);
                processVector.erase(it, it + 1);
                it--;
            } else break;
        }
        if(process_executing) {
            std::for_each(processQueue.begin() + 1, processQueue.end(),
                          [](process_info& pi) {pi.wait_time++; pi.turnaround_time++;});
            processQueue.front().quantum_remaining--;
            processQueue.front().turnaround_time++;
            processQueue.front().burst_time--;
            if(processQueue.front().burst_time == 0) {
                std::cout << "\tProcess Terminated" << std::endl;
                process_stats[processQueue.front().processID] = std::make_pair(processQueue.front().wait_time, processQueue.front().turnaround_time);
                processQueue.erase(processQueue.begin());
                process_executing = false;
            } else if(processQueue.front().quantum_remaining == 0) {
                std::cout << "\tQuantum Expired" << std::endl;
                processQueue.push_back(processQueue.front());
                processQueue.erase(processQueue.begin());
                process_executing = false;
            }
        }
        if(!process_executing && !processQueue.empty()) {
            std::cout << current_time << " " << processQueue.front().processID;
            process_executing = true;
            processQueue.front().quantum_remaining = quantum;
        }
        if(!process_executing && processVector.empty()) {
            std::cout << current_time << "\tComplete\n" << std::endl;
            auto total_wait_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                   [](int s, const std::pair<char, std::pair<int, int>> &p) {
                                                       return s + p.second.first;
                                                   });
            auto total_turnaround_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                         [](int s, const std::pair<char, std::pair<int, int>> &p) {
                                                             return s + p.second.second;
                                                         });

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            std::cout << std::left << std::setw(13) << "Process ID" << std::setw(18) << "Turnaround Time"
                      << "Waiting Time" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            for (const auto &elem: process_stats) {
                std::cout << std::left << std::setw(13) << elem.first << std::setw(18) << elem.second.second
                          << elem.second.first << std::endl;
            }
            std::cout << std::left << std::setw(13) << "Average" << std::setw(18)
                      << (float) total_turnaround_time / process_stats.size();
            std::cout << (float) total_wait_time / process_stats.size() << std::endl << std::endl;
            break;
        }
        current_time++;
    }
}

void shortest_remaining_time_first(std::vector<process_info> processVector) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),13);
    std::cout << "SRTF Scheduling" << std::endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
    std::map<char, std::pair<int, int>> process_stats;
    auto compare = [](const process_info& lhs, const process_info& rhs) {
        return lhs.burst_time > rhs.burst_time;
    };
    bool process_executing{false};
    std::vector<process_info> processQueue;
    process_info current_process;
    int current_time{0};
    std::sort(processVector.begin(), processVector.end(),
              [](const process_info& pi1, const process_info& pi2) {
                  return pi1.arrival_time < pi2.arrival_time;
              });
    while(true) {
        for(auto it = processVector.begin(); it != processVector.end(); it++) {
            if(it->arrival_time == current_time) {
                processQueue.push_back(*it);
                processVector.erase(it, it + 1);
                it--;
            } else {
                std::make_heap(processQueue.begin(), processQueue.end(), compare);
                break;
            }
        }
        if(process_executing) {
            std::for_each(processQueue.begin(), processQueue.end(),
                          [](process_info& pi) {pi.wait_time++; pi.turnaround_time++;});
            current_process.turnaround_time++;
            current_process.burst_time--;
            if(current_process.burst_time == 0) {
                std::cout << "\tProcess Terminated" << std::endl;
                process_stats[current_process.processID] = std::make_pair(current_process.wait_time, current_process.turnaround_time);
                process_executing = false;
            }
            if(processQueue.front().burst_time < current_process.burst_time) {
                processQueue.push_back(current_process);
                current_process = processQueue.front();
                processQueue.erase(processQueue.begin());
                std::make_heap(processQueue.begin(), processQueue.end(), compare);
            }
        }
        if(!process_executing && !processQueue.empty()) {
            current_process = processQueue.front();
            processQueue.erase(processQueue.begin());
            std::make_heap(processQueue.begin(), processQueue.end(), compare);
            std::cout << current_time << " " << current_process.processID;
            process_executing = true;
        }
        if(!process_executing && processVector.empty()) {
            std::cout << current_time << "\tComplete\n" << std::endl;
            auto total_wait_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                   [](int s, const std::pair<char, std::pair<int, int>> &p) {
                                                       return s + p.second.first;
                                                   });
            auto total_turnaround_time = std::accumulate(process_stats.begin(), process_stats.end(), 0,
                                                         [](int s, const std::pair<char, std::pair<int, int>> &p) {
                                                             return s + p.second.second;
                                                         });

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
            std::cout << std::left << std::setw(13) << "Process ID" << std::setw(18) << "Turnaround Time"
                      << "Waiting Time" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            for (const auto &elem: process_stats) {
                std::cout << std::left << std::setw(13) << elem.first << std::setw(18) << elem.second.second
                          << elem.second.first << std::endl;
            }
            std::cout << std::left << std::setw(13) << "Average" << std::setw(18)
                      << (float) total_turnaround_time / process_stats.size();
            std::cout << (float) total_wait_time / process_stats.size() << std::endl << std::endl;
            break;
        }
        current_time++;
    }
}