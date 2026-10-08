__kernel void matmul(__global float* mat1, __global float* mat2_tns, __global int* sizes, __global float* result) {
	int i = get_global_id(0);
	int k = get_global_id(1);
	
	float sum = 0;
	if (i < sizes[0] && k < sizes[2]) {
		for (int j = 0; j < sizes[1]; ++j) {
			sum = sum + mat1[i * sizes[1] + j] * mat2_tns[k * sizes[1] + j];
		}
	}
	result[i * sizes[2] + k] = sum;
}
