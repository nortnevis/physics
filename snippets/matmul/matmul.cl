__kernel void matmul(__global float* mat1, __global float* mat2, __global int* sizes, __global float* result) {
	 int i = get_global_id(0);
	 int j = get_global_id(1);
	 int k = get_global_id(1);

	 if (i < sizes[0] && j < sizes[1] && k < sizes[2]) {
		int c;
	 }
}
