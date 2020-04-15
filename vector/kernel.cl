#pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef double __attribute__ ((aligned(16*sizeof(double)))) aligned_double;

kernel void vector_add(global aligned_double* A,global aligned_double* B,global aligned_double* C,unsigned int N,unsigned int Nthreads)
{
unsigned int thread_id = get_global_id(0);
unsigned int thread_space = N / Nthreads;

unsigned int start = thread_space * thread_id;
unsigned int stop = (thread_id == Nthreads - 1) ? N : (thread_space * (thread_id + 1));

global double16* fast_A = (global double16*)A; 
global double16* fast_B = (global double16*)B; 
global double16* fast_C = (global double16*)C; 

for(int j=0; j<10000; j++)
{
for(unsigned int i=start/16; i<stop/16; i++){fast_C[i] = fast_A[i] + fast_B[i];}
}
 
}
