__kernel void matmul(__global float* mat1, __global float* mat2_tns, __global int* sizes, __global float* result) {
	int i = get_global_id(0);
	int k = get_global_id(1);
	
	if (i < sizes[0] && k < sizes[2]) {
		for (int j = 0; j < sizes[1]; ++j) {
			printf("[%i,%i] = [%i,%i] x [%i,%i]\n", i, j, i, j, j, k);
			float dot = mat1[i * sizes[1] + j] * mat2_tns[k + sizes[2] * j];
			printf("%f = %f x %f\n", dot, mat1[i * sizes[1] + j], mat2_tns[k + sizes[2] * j]);
			result[i * sizes[2] + j] = result[i * sizes[2] + j] + dot;
		}
	}
}
