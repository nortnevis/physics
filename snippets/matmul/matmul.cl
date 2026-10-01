__kernel void matmul(__global float* mat1, __global float* mat2_tns, __global int* sizes, __global float* result) {
	int i = get_global_id(0);
	int j = get_global_id(1);
	int k = get_global_id(2);
	if (i < sizes[0] && j < sizes[1] && k < sizes[2]) {
		printf("[%i, %i] = [%i, %i] x [%i, %i]\n", i, j, i, k, k, j);
		__private float dot = mat1[i * sizes[0] + k] * mat2_tns[k * sizes[2] + j];
		printf("%f = %f x %f\n", dot, mat1[i * sizes[0] + k], mat2_tns[k * sizes[2] + j]);
		result[i * sizes[0] + j] = result[i * sizes[0] + j]  + dot;
		printf("result: %f\n\n", result[i * sizes[0] + j]);
	}
}
