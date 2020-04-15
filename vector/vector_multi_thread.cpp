#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>

#include <pthread.h>

volatile double *A,*B,*C; // vectorii A,B si C sunt pointeri

// parametri reali ai threadului
struct thread_argv {
    unsigned int start;
    unsigned int stop;
};

void* thread_proc(void* arg) {
    struct thread_argv* argv = (struct thread_argv*)arg;
    for(int j=0; j<10000; j++) {
    // sa fie fair fata de single core
        for(unsigned int i= argv->start; i< argv->stop; i++) {
            C[i] = A[i] + B[i];
        }
    }
}

const char* usage = "Usage: vector_multi_thread.exe N";
int main(int argc,char** argv) {
    if(argc != 2) {
        std::cout << usage << std::endl;
        return 0;
    }
    unsigned int N;

    int r = sscanf(argv[1],"%d",&N);
    if(r != 1){std::cerr << argv[1] << " nu este un numar valid!" << std::endl; return -1;}

    std::cout << "Se aloca memorie pentru vectori!" << std::endl; 
    A = new (std::nothrow) double[N * 3]; // se aloca o singura data toti cei 3 vectori
    if(A == NULL) {
        std::cerr << "Nu se poate aloca memorie pentru cei trei vectori!" << std::endl; return -1;
    }
    B = A + N;
    C = B + N;
    std::cout << "Se genereaza aleator vectorii A si B!" << std::endl;
    srand(time(NULL));
    for(unsigned int i=0; i<N; i++){A[i] = rand();}
    for(unsigned int i=0; i<N; i++){B[i] = rand();}

    std::cout << "Se va calcula C = A + B;" << std::endl; 

    pthread_t threads[64];
    struct thread_argv t_info[64];
    unsigned int thread_num = std::thread::hardware_concurrency(); // numar de procesoare logice

    // se limiteaza la 64 de threaduri;
    if(thread_num > 64){thread_num = 64;} 

    std::cout << "Se calculeaza cu " <<  thread_num << " thread-uri!" << std::endl;

    struct timespec start, end;

    unsigned int thread_space = N / thread_num; // intervalul din vetor pe care o calculeaza un thread

    for(unsigned int i=0; i<thread_num; i++) {
        t_info[i].start = thread_space * i;
        t_info[i].stop = thread_space * (i+1); 

        // e vorba de ultima parte si e posibil sa nu se divida complet
        //algoritmul va face in asa fel incat daca mai ramane ceva, ultimul thread va primi partea mai mare
        if(i == thread_num - 1) {t_info[i].stop = N;}
        pthread_create(&threads[i],NULL,thread_proc,(void*)&t_info[i]);
    }


        clock_gettime(CLOCK_MONOTONIC,&start);

    for(unsigned int i=0; i<thread_num; i++) 
    {// se asteapta sa termine toate threadurile
        pthread_join(threads[i],NULL);
    }

    clock_gettime(CLOCK_MONOTONIC,&end);
    double duration = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/1000000000.0);

    std::cout << "Suma vectorilor a durat "<< duration << " secunde!" << std::endl;

    delete[] A;
    return 0;
}
