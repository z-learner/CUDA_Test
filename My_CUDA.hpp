#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <sys/time.h>
#include <stdio.h>
#define CUDACHECK(call) \
{\
 const cudaError_t error = call;\
 if (error != cudaSuccess) {\
    printf("Error occurs, location is %s : %d\n", __FILE__, __LINE__); \
    printf("Error occurs, Reason is : %s , and Code is : %d\n", cudaGetErrorString(error) , error);\
    exit(1); \
 }\
}

inline 
bool checkCudaError(cudaError_t status) {
   if (status != cudaSuccess) {
      printf("Error occurs, location is %s : %d\n", __FILE__, __LINE__); 
      printf("Error occurs, Reason is : %s , and Code is : %d\n", cudaGetErrorString(status) , status);
      return true;
   }
   return false;
}


double cpuSecond() {
   struct timeval tp;
   gettimeofday(&tp,NULL);
   return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

template <typename T> 
bool judgeMatrixBetweenCpuAndGpuResult(T* cpu, T* gpu, unsigned int nx, unsigned int ny) {
   for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            int index = i * ny + j;
            if (abs(cpu[i] - gpu[i]) > 1.e-6) return false;
        }
    }
    return true;
}

template <typename T>
bool judgeArrayBetweenCpuAndGpuResult(T* cpu, T* gpu, unsigned int size) {
   for (int i = 0; i < size; ++i) {
      if (abs(cpu[i] - gpu[i]) > 1.e-6) return false;
   }
   return true;
}


void setDevice(int dev) {
   cudaDeviceProp deviceProp;
   CUDACHECK(cudaGetDeviceProperties(&deviceProp, dev));
   printf("Using Device %d : %s\n", dev, deviceProp.name);
   CUDACHECK(cudaSetDevice(dev));
}

template<typename T>
void initMem(T* ptr, unsigned int size) {
   srand(time(NULL));
   for (int i = 0; i < size; ++i) {
      ptr[i] = rand() & 0XFF;
   }
}

void myPrintfFloat(float* in, unsigned int size) {
   for (int i = 0; i < size; ++i) {
      printf("%f ", in[i]);
   }
   printf("\n");
}


void myPrintfFloatGpu(float* in, unsigned int size) {
   for (int i = 0; i < size; ++i) {
      float temp = 0;
      cudaMemcpy(&temp, &in[i], sizeof(float), cudaMemcpyDeviceToHost);
      printf("%f ", temp);
   }
   printf("\n");
}


cudaDeviceProp simpleDeviceQuery(int dev = 0) {
   cudaDeviceProp deviceProp;
   CUDACHECK(cudaGetDeviceProperties(&deviceProp, dev));
   printf("Using Device %d : %s\n", dev, deviceProp.name);
   printf("Number of multiprocessors : %d\n", deviceProp.multiProcessorCount);
   printf("Total amount of constant memory : %4.2f KB\n", deviceProp.totalConstMem / 1024.0);
   printf("Total amount of shared memory per block : %4.2f KB\n", deviceProp.sharedMemPerBlock / 1024.0);
   printf("Total Number of registers available per block : %d\n", deviceProp.regsPerBlock);
   printf("Warp size %d\n", deviceProp.warpSize);
   printf("Maximum number of threads per block : %d\n", deviceProp.maxThreadsPerBlock);
   printf("Maximum number of threads per multiprocessor :%d\n", deviceProp.maxThreadsPerMultiProcessor);
   printf("Maximum number of warps per multiprocessor : %d\n", deviceProp.maxThreadsPerMultiProcessor / 32); 
   return deviceProp;}