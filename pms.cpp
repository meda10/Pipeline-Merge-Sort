#include <iostream>
#include <mpi.h>
#include <cmath>
#include <queue>
#include <fstream>
#include <cstdint>
#include <stack>

#define QUEUE_1_TAG 0
#define QUEUE_2_TAG 1
//#define TIME

/**
 * Function measures time difference
 * Function is taken from: https://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * @return time difference
 */
timespec diff(timespec start, timespec end){
    timespec temp{};
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}


/**
 * Reads file called numbers
 * @return  queue that consists of read numbers
 */
std::queue<uint8_t> read_numbers_file(){
    std::queue<uint8_t> numbers;
    std::fstream file;
    file.open("numbers", std::ios::in);
    if (file.is_open()){
        while (file.good()){
            int num = file.get();
            if(!file.good()){
                break;
            }
            numbers.push(num);
        }
        file.close();
    } else {
        std::cerr << "Can not open file numbers";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    return  numbers;
}


int main(int argc, char *argv[]) {
    timespec time1{}, time2{};
    int number_of_processors;
    int my_id;

    //MPI init
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&number_of_processors);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_id);

    double input_size_all = pow(2, number_of_processors - 1);

    //First processor -> reads file numbers and sends the values to next processors queue_1 and queue_2
    if(my_id == 0) {
        std::queue<uint8_t> numbers = read_numbers_file();
        std::queue<uint8_t> print_numbers = numbers;

        if(input_size_all != numbers.size()){
            MPI_Abort(MPI_COMM_WORLD, 5);
        }

#ifndef TIME
        //Print numbers
        while (!print_numbers.empty()) {
            std::cout << unsigned(print_numbers.front()) << ' ';
            print_numbers.pop();
        }
        std::cout << std::endl;
#else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
#endif

        //Send numbers to next processor -> resulting queue is based on tag
        int i = 0;
        int tag = 0;
        while (!numbers.empty()) {
            uint8_t number = numbers.front();
            numbers.pop();
            MPI_Send(&number, 1, MPI_INT8_T, my_id + 1, tag, MPI_COMM_WORLD);
            tag = (tag + 1) % 2;
            i++;
        }
    } else {
        std::queue<uint8_t> queue_1;
        std::queue<uint8_t> queue_2;
        std::stack<uint8_t> output;

        MPI_Status status;
        int tag = QUEUE_1_TAG;
        bool start_sending_numbers = false;
        double input_size = pow(2, my_id - 1);
        double output_size = pow(2, my_id);
        int received_numbers = 0;
        double k = 1;
        int m = 1;
        double q1_num = input_size;
        double q2_num = input_size;

        while (k <= input_size_all) {
            //Receive numbers from predecessor, put them to queue_1 or queue_2 based on tag
            if (received_numbers < input_size_all) {
                uint8_t number;
                MPI_Recv(&number, 1, MPI_INT8_T, my_id - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if (status.MPI_TAG == QUEUE_1_TAG) {
                    queue_1.push(number);
                } else if (status.MPI_TAG == QUEUE_2_TAG) {
                    queue_2.push(number);
                }
                //processor have enough numbers -> it can start comparing and sending them to next processor
                if (queue_1.size() == input_size && queue_2.size() == 1) {
                    start_sending_numbers = true;
                }
                received_numbers++;
            }
            if (start_sending_numbers) {
                // Middle processors -> compare first elements from queue_1 and queue_2, remove larger one
                // and send it to next processors queue (number of queue is based on variable tag)
                if (my_id != number_of_processors - 1) {
                    if ((queue_1.front() <= queue_2.front() && !queue_2.empty()) || queue_1.empty() || q1_num == 0) {
                        uint8_t number = queue_2.front();
                        queue_2.pop();
                        MPI_Send(&number, 1, MPI_INT8_T, my_id + 1, tag, MPI_COMM_WORLD);
                        q2_num--;
                        m++;
                    } else if ((queue_1.front() > queue_2.front() && !queue_1.empty()) || queue_2.empty() || q2_num == 0) {
                        uint8_t number = queue_1.front();
                        queue_1.pop();
                        MPI_Send(&number, 1, MPI_INT8_T, my_id + 1, tag, MPI_COMM_WORLD);
                        q1_num--;
                        m++;
                    } else {
                        MPI_Abort(MPI_COMM_WORLD, 2);
                    }
                    if (m > output_size) {
                        m = 1;
                        q1_num = input_size;
                        q2_num = input_size;
                        tag = (tag + 1) % 2;
                        k = k + output_size;
                    }
                //Last processor -> compare first elements from queue_1 and queue_2, remove larger one
                // and saves it to variable output
                } else {
                    if ((queue_1.front() <= queue_2.front() && !queue_2.empty()) || queue_1.empty()) {
                        uint8_t number = queue_2.front();
                        queue_2.pop();
                        output.push(number);
                        m++;
                    } else if ((queue_1.front() > queue_2.front() && !queue_1.empty()) || queue_2.empty()) {
                        uint8_t number = queue_1.front();
                        queue_1.pop();
                        output.push(number);
                        m++;
                    } else {
                        MPI_Abort(MPI_COMM_WORLD, 2);
                    }
                    if (m > output_size) {
                        m = 1;
                        k = k + output_size;
                    }
                }
            }
        }
#ifdef TIME
        //Send message to process 0 that numbers are sorted and time measurement can stop
        uint8_t number = 0;
        MPI_Send(&number, 1, MPI_INT8_T, 0, 0, MPI_COMM_WORLD);
#else
        //Print sorted numbers
        if (my_id == number_of_processors - 1) {
            while (!output.empty()) {
                std::cout << unsigned(output.top()) << '\n';
                output.pop();
            }
        }
#endif
    }

#ifdef TIME
    //End time measurement and print results
    if (my_id == 0) {
        uint8_t number;
        MPI_Recv(&number, 1, MPI_INT8_T, number_of_processors - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        timespec time = diff(time1, time2);
        std::cout << time.tv_sec << ":" << time.tv_nsec << std::endl;
    }
#endif

    MPI_Finalize();
    return 0;
}