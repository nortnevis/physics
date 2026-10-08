#include "render.hpp"

#if defined(_MSC_VER)
#define DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable : 4996))
#define RESTORE_DEPRECATION_WARNINGS __pragma(warning(pop))
#elif defined(__GNUC__) || defined(__clang__)
#define DISABLE_DEPRECATION_WARNINGS                                                                                   \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define RESTORE_DEPRECATION_WARNINGS _Pragma("GCC diagnostic pop")
#else
#define DISABLE_DEPRECATION_WARNINGS
#define RESTORE_DEPRECATION_WARNINGS
#endif

cl_device_id create_device();

cl_program build_kernel_code(cl_context ctx);

int align(int x, int y);

void invoke_kernel(cl_kernel kernel, cl_command_queue queue, cl_mem buff, cl_uint *result, float x, float y, float mag,
                   int w, int h, float iters);

void gpu_calculus(std::stop_token stop, std::vector<cl_uint> &pixels, int res_w, int res_h);

int main(int, char **) {
    static const int res_w = 1200;
    static const int res_h = 640;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(res_w, res_h, "Mandelbrot_set");
    SetTargetFPS(60);

    std::vector<cl_uint> pixels(res_w * res_h);

    std::jthread gpu_thr{[&](std::stop_token stop) { gpu_calculus(stop, pixels, res_w, res_h); }};
    render_loop(pixels, res_w, res_h);

    CloseWindow();
    return 0;
}

void gpu_calculus(std::stop_token stop, std::vector<cl_uint> &pixels, int res_w, int res_h) {
    auto device = create_device();
    cl_int err;
    auto context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err)
        throw;
    auto program = build_kernel_code(context);
    auto kernel = clCreateKernel(program, "draw_mandelbrot", &err);
    if (err)
        throw;
    DISABLE_DEPRECATION_WARNINGS
    auto command_queue = clCreateCommandQueue(context, device, 0, &err);
    RESTORE_DEPRECATION_WARNINGS
    if (err)
        throw;
    auto cl_mem_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint) * res_w * res_h, nullptr, &err);
    if (err)
        throw;

    // Allocate memory for the extension string
    size_t ext_size;
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &ext_size);
    char *extensions = (char *)malloc(ext_size);

    // Fetch the supported extensions
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, ext_size, extensions, NULL);

    // Check for the double precision string
    if (strstr(extensions, "cl_khr_fp64") != NULL) {
        printf("Double precision is supported!\n");
    } else {
        printf("Double precision is NOT supported.\n");
    }

    free(extensions);

    float xcenter = 0.f;
    float ycenter = 0.f;
    constexpr float center_step = 0.0005f;

    float scale = 1.0f;
    constexpr float scale_step = 0.01f;

    while (!stop.stop_requested()) {
        if (IsKeyDown(KEY_E)) {
            scale += scale_step;
        }
        if (IsKeyDown(KEY_Q)) {
            scale -= scale_step;
        }
        if (IsKeyDown(KEY_D)) {
            xcenter += center_step;
        }
        if (IsKeyDown(KEY_A)) {
            xcenter -= center_step;
        }
        if (IsKeyDown(KEY_W)) {
            ycenter += center_step;
        }
        if (IsKeyDown(KEY_S)) {
            ycenter -= center_step;
        }

        invoke_kernel(kernel, command_queue, cl_mem_buff, pixels.data(), xcenter, ycenter, scale, res_w, res_h, 50);
    }
    clReleaseKernel(kernel);
    clReleaseMemObject(cl_mem_buff);
    clReleaseCommandQueue(command_queue);
    clReleaseProgram(program);
    clReleaseContext(context);
}

cl_device_id create_device() {
    cl_platform_id platform;
    cl_device_id dev;
    cl_int err = 0;
    err |= clGetPlatformIDs(1, &platform, nullptr);
    err |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, nullptr);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, nullptr);
    }
    if (err)
        throw;
    return dev;
}

cl_program build_kernel_code(cl_context ctx) {
    cl_int err = 0;

    std::ifstream cl_file("mandelbrot_set.cl");
    std::stringstream buffer;
    buffer << cl_file.rdbuf();
    const char *source_code = buffer.view().data();
    size_t source_code_size = buffer.view().size();
    auto program = clCreateProgramWithSource(ctx, 1, &source_code, &source_code_size, &err);
    err |= clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err)
        throw;
    return program;
}

void invoke_kernel(cl_kernel kernel, cl_command_queue queue, cl_mem buff, cl_uint *result, float x, float y, float mag,
                   int w, int h, float iters) {
    cl_int err = 0;
    err |= clSetKernelArg(kernel, 0, sizeof(float), &x);
    err |= clSetKernelArg(kernel, 1, sizeof(float), &y);
    err |= clSetKernelArg(kernel, 2, sizeof(float), &mag);
    err |= clSetKernelArg(kernel, 3, sizeof(float), &iters);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_int), &w);
    err |= clSetKernelArg(kernel, 5, sizeof(cl_int), &h);
    err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &buff);
    err |= clSetKernelArg(kernel, 7, sizeof(cl_int), &w);

    // workgroup sizes: 256x1
    size_t local_size[2] = {256, 1};
    size_t global_size[2] = {
        (size_t)align(w, (int)local_size[0]),
        (size_t)align(h, (int)local_size[1]),
    };

    // run code
    err |= clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, global_size, local_size, 0, nullptr, nullptr);

    // read result
    err |= clEnqueueReadBuffer(queue, buff, CL_TRUE, 0, sizeof(cl_uint) * w * h, result, 0, nullptr, nullptr);
    clFinish(queue);
}

int align(int x, int y) {
    // we use this formula to align x to y
    return (x + y - 1) / y * y;
}
