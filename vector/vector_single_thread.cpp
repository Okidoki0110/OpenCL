#include <iostream>
#include <cstdlib>
#include <ctime>

volatile double *A,*B,*C; // vectorii A,B si C sunt pointeri

const char* usage = "Usage: vector_single_thread.exe N";

int main(int argc,char** argv)
{
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
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC,&start);

    // se calculeaza de 10000 de ori, daca nu s-ar pune asta aici,
    //programul ar iesi foarte repede fara a avea un timp de executie precis/relevant
    for(int j=0; j<10000; j++) {
        for(unsigned int i=0; i<N; i++)
            {C[i] = A[i] + B[i];}
    }

    clock_gettime(CLOCK_MONOTONIC,&end);
    double duration = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/1000000000.0);
    std::cout << "Suma vectorilor a durat "<< duration << " secunde!" << std::endl;
    
    delete[] A;
    return 0;
}
