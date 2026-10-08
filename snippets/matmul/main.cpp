#include "other.hpp"

std::vector<float> control_calc(const std::vector<float> &mat1, const std::vector<float> &mat2,
                                const std::vector<int> &mat_sizes) {
    namespace ch = std::chrono;
    std::vector<float> result(mat_sizes.at(0) * mat_sizes.at(2));
    auto start = ch::steady_clock::now();
    for (int row_a = 0; row_a < mat_sizes.at(0); ++row_a) {
        for (int col_b = 0; col_b < mat_sizes.at(2); ++col_b) {
            float sum = 0;
            for (int el = 0; el < mat_sizes.at(1); ++el) {
                sum += mat1[el + row_a * mat_sizes.at(1)] * mat2[col_b + el * mat_sizes.at(2)];
            }
            result[row_a * mat_sizes.at(2) + col_b] = sum;
        }
    }
    auto end = ch::steady_clock::now();
    std::println("Control on cpu calculation: {} ms", ch::duration_cast<ch::milliseconds>(end - start).count());
    return result;
}

std::vector<float> gpu_calc(const std::vector<float> &mat1, const std::vector<float> &mat2,
                            const std::vector<int> &mat_sizes) {
    cl::Context context;
    auto dev_result = ph::get_device(context);
    if (!dev_result.has_value()) {
        switch (dev_result.error()) {
        case ph::DeviceError::ComputeDeviceIsNotAwailable:
            std::println("Compute device is not awailable.");
            break;
        case ph::DeviceError::DeviceNameIsNotAwailable:
            std::println("Device name is not awailable.");
            break;
        case ph::DeviceError::DeviceVendorIsNotAwailable:
            std::println("Device vendor is not awailable.");
            break;
        default:
            std::println("Unknown error on get_device()");
            break;
        }
        std::terminate();
    }
    auto &dev = dev_result.value();

    std::filesystem::path kernel_path("matmul.cl");
    auto program_result = ph::compile_kernel(kernel_path, context, dev);
    if (!program_result.has_value()) {
        switch (program_result.error()) {
        case ph::KernelError::FailedToOpenKernelCodeTextFile:
            std::println("Failed to open kernel file '{}'.", kernel_path.string());
            break;
        default:
            std::println("Unknown error on compile_kernel()");
            break;
        }
        std::terminate();
    }
    auto &program = program_result.value();
    cl::Kernel kernel(program, "matmul");
    cl::CommandQueue cmd_queue(context, dev);

    auto mat2_tns = transpose(mat2, mat_sizes.at(1), mat_sizes.at(2));

    cl::Buffer mat1_buff(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, mat1.size() * sizeof(float),
                         (void *)mat1.data());
    cl::Buffer mat2_tns_buff(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, mat2_tns.size() * sizeof(float),
                             (void *)mat2_tns.data());
    cl::Buffer sizes_buff(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, mat_sizes.size() * sizeof(int),
                          (void *)mat_sizes.data());
    std::vector<float> result(mat_sizes.at(0) * mat_sizes.at(2));
    cl::Buffer result_buff(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, result.size() * sizeof(float),
                           (void *)result.data());

    kernel.setArg(0, mat1_buff);
    kernel.setArg(1, mat2_tns_buff);
    kernel.setArg(2, sizes_buff);
    kernel.setArg(3, result_buff);

    auto [local_range, global_range] = ph::get_task_ndranges(dev, std::vector{mat_sizes.at(0), mat_sizes.at(2)});

    std::println("Task ndragnes:");
    std::println("local dims: {}; Sizes: ", local_range.dimensions());
    const auto *local = local_range.get();
    for (int i = 0; i < local_range.dimensions(); ++i) {
        std::cout << " " << local[i];
    }
    std::cout << std::endl;

    std::println("global dims: {}; Sizes", global_range.dimensions());
    const auto *global = local_range.get();
    for (int i = 0; i < local_range.dimensions(); ++i) {
        std::cout << " " << global[i];
    }
    std::cout << std::endl;

    namespace ch = std::chrono;
    auto start = ch::steady_clock::now();
    cmd_queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_range, local_range);
    cmd_queue.finish();
    cmd_queue.enqueueReadBuffer(result_buff, CL_TRUE, 0, sizeof(float) * result.size(), (void *)result.data());

    auto end = ch::steady_clock::now();
    std::println("Control on gpu calculation: {} ms", ch::duration_cast<ch::milliseconds>(end - start).count());

    return result;
}

int main(int argc, const char *argv[]) {
    try {
        ph::seed(1);
        auto mat_sizes = parse_args(argc, argv);

        std::vector<float> mat1(mat_sizes.at(0) * mat_sizes.at(1));
        std::vector<float> mat2(mat_sizes.at(1) * mat_sizes.at(2));

        init(mat1);
        init(mat2);

        auto gpu_result = gpu_calc(mat1, mat2, mat_sizes);

        if (mat_sizes.at(0) * mat_sizes.at(1) * mat_sizes.at(2) < 100000) {
            auto cpu_result = control_calc(mat1, mat2, mat_sizes);
            std::println("\nVec1:");
            print(mat1, mat_sizes.at(0), mat_sizes.at(1));
            std::println("\nVec2:");
            print(mat2, mat_sizes.at(1), mat_sizes.at(2));
            std::println("\nProduct:");
            print(cpu_result, mat_sizes.at(0), mat_sizes.at(2));

            std::println("\nGPU Product:");
            print(gpu_result, mat_sizes.at(0), mat_sizes.at(2));
            auto eq = is_equal(cpu_result, gpu_result);
            std::println("\nIs correct: {}", eq ? "true" : "false");
        }

    } catch (const std::exception &e) {
        std::println("{}", e.what());
        return -1;
    }
    return 0;
}
