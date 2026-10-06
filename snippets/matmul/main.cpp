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
    auto dev = ph::get_deivce(context, mode);
    auto program = ph::compile_kernel("matmul.cl", context, dev);
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

    std::print("mat_sizes:");
    for (const auto &sz : mat_sizes) {
        std::print(" {}", sz);
    }
    std::println("");

    kernel.setArg(0, mat1_buff);
    kernel.setArg(1, mat2_tns_buff);
    kernel.setArg(2, sizes_buff);
    kernel.setArg(3, result_buff);

    auto [local_range, global_range] = ph::get_task_ndranges(dev, std::vector(mat_sizes.at(0), mat_sizes.at(2)));

    std::println("Task ndragnes:");
    std::println("local. dims: {}, size: {}", local_range.dimensions(), local_range.size());
    std::println("global. dims: {}, size: {}", global_range.dimensions(), global_range.size());
    std::print("local =");
    const auto *local = local_range.get();
    for (int i = 0; i < local_range.dimensions(); ++i) {
        std::cout << " " << local[i];
    }
    std::cout << std::endl;
    std::print("global =");
    const auto *global = local_range.get();
    for (int i = 0; i < local_range.dimensions(); ++i) {
        std::cout << " " << global[i];
    }
    std::cout << std::endl;

    cmd_queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_range, local_range);
    std::println("{}", __LINE__);
    cmd_queue.finish();
    std::println("{}", __LINE__);
    cmd_queue.enqueueReadBuffer(result_buff, CL_TRUE, 0, sizeof(float) * result.size(), (void *)result.data());
    std::println("{}", __LINE__);

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

        std::println("{}", __LINE__);
        if (mat_sizes.at(0) * mat_sizes.at(1) * mat_sizes.at(2) < 1000) {
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
