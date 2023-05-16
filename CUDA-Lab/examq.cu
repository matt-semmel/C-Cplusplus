float* d_A = NULL;      // Pointer to matrix A in device memory
float* h_A = NULL;      // Pointer to matrix A in host memory
int N = 1024;           // X and y dimesions of matrix A
int BLOCK_SIZE = 32;    // X and y dimesions of a thread block

void inc_cpu(float* A, int n) {
        for (int i=0; i<n; i++) {
                for (int j=0; j<n; j++) {
                        A[i*n + j] += 1;
                }
        }
}

__global__ void inc_gpu(float* A, int n)
{
        // 1. TODO: Implement
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		for (int j = 0; j < n; j++)
			A[i*n + j] += 1
	}
}

int main(int argc, char** argv)
{
        int policy = atoi(argv[1]);             // 1 or 2

        float* h_A = (float*)malloc(sizeof(float) * N * N);
        memset(h_A, 0, sizeof(float) * N * N);

        if(policy==1){ // CPU version
                inc_cpu(h_A, N);
        }
        else if(policy==2){ // GPU version
                // 2. TODO: Set up GPU memory and copy input data
				cudaMalloc((void**) &d_A, (n * n * sizeof(float)));
				cudaMemcpy(d_A, h_A, sizeof(float) * n * n, cudaMemcpyHostToDevice);

                // 3. TODO: Launch kernel inc_gpu
			
        dim2 dimBlock(block_size, block_size);
        dim2 dimGrid(N / dimBlock.x, N / dimBlock.y);
        mm_gpu <<<dimGrid, dimBlock, sizeof(float) * block_size * block_size * 2>>> (d_A, N);

                cudaDeviceSynchronize();

                // 4. TODO: Copy back result data and tear down GPU memory
				cudaMemcpy(h_A, d_A, (sizeof(float) * n * n), cudaMemcpyDeviceToHost);
    			cudaFree(d_A);
        }

        free(h_A);
        return 0;
}