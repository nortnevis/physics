static auto mode = ph::Mode::GPU;

decltype(auto) parse_args(int argc, const char *argv[]) {
    std::vector mat_sizes{5, 5, 5};
    constexpr std::string_view error_msg = "Unknown arg '{}' at pos {}!";
    constexpr int mat_max_args = 3;
    bool are_sizes_sequential = true;
    for (int pos = 1; pos < argc; ++pos) {
        std::string_view arg = argv[pos];
        if (pos <= mat_max_args && are_sizes_sequential && ph::is_number(arg)) {
            mat_sizes.at(pos - 1) = std::stoi(std::string(arg));
        } else if (arg == "-h" || arg == "--help") {
            std::println("Awailable args: ([dim1] [dim2] [dim3]) [--cpu] [--gpu] [--help] [-h]");
            std::exit(0);
        } else if (arg == "--cpu") {
            if (pos <= mat_max_args) {
                are_sizes_sequential = false;
            }
            mode = ph::Mode::CPU;
        } else if (arg == "--gpu") {
            if (pos <= mat_max_args) {
                are_sizes_sequential = false;
            }
            mode = ph::Mode::GPU;
        } else {
            throw std::runtime_error{std::format(error_msg, argv[pos], pos)};
        }
    }

    std::print("Sizes are: ");
    for (const auto &s : mat_sizes) {
        std::print("{} ", s);
    }
    std::println("");

    return mat_sizes;
}

void init(std::vector<float> &mat) {
    for (auto &el : mat) {
        el = ph::rand_int(0, 10);
    }
}

void print(const std::vector<float> &mat, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::print("{} ", mat[i * rows + j]);
        }
        std::println("");
    }
}

std::vector<float> control_calc(const std::vector<float> &mat1, const std::vector<float> &mat2,
                                const std::vector<int> &mat_sizes) {
    namespace ch = std::chrono;
    std::vector<float> result(mat_sizes.at(0) * mat_sizes.at(2));
    auto start = ch::steady_clock::now();
    for (int row_a = 0; row_a < mat_sizes.at(0); ++row_a) {
        for (int col_b = 0; col_b < mat_sizes.at(2); ++col_b) {
            float sum = 0;
            for (int el = 0; el < mat_sizes.at(1); ++el) {
                sum += mat1[el + row_a * mat_sizes.at(0)] * mat2[col_b + el * mat_sizes.at(1)];
            }
            result[row_a * mat_sizes.at(0) + col_b] = sum;
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

    cl::Buffer mat1_buff(context, CL_MEM_HOST_PTR | CL_MEM_READ_ONLY, mat1.size() * sizeof(float), (void *)mat1.data());
    cl::Buffer mat2_buff(context, CL_MEM_HOST_PTR | CL_MEM_READ_ONLY, mat2.size() * sizeof(float), (void *)mat2.data());

    std::vector<float> result(mat_sizes.at(0) * mat_sizes.at(2));
    cl::Buffer result_buff(context, CL_MEM_HOST_PTR | CL_MEM_WRITE_ONLY, result.size() * sizeof(float),
                           (void *)result.data());

    kernel.setArg(0, mat1_buff);
    kernel.setArg(1, mat2_buff);
    kernel.setArg(2, result_buff);

    cl::NDRange local_range;
    cl::NDRange global_range;

    cmd_queue.enqueueNDRangeKernel(kernel, cl::NullRange, local_range, global_range);
    cmd_queue.finish();
    cmd_queue.enqueueReadBuffer(result_buff, CL_TRUE, 0, sizeof(float) * result.size(), (void *)result.size());

    return result;
}

bool is_equal(std::vector<float> &l, std::vector<float> &r) {
    if (l.size() != r.size()) {
        return false;
    }
    for (int i = 0; i < l.size(); ++i) {
        if (l[i] != r[i]) {
            return false;
        }
    }
    return true;
}

int main(int argc, const char *argv[]) {
    try {
        auto mat_sizes = parse_args(argc, argv);

        std::vector<float> mat1(mat_sizes.at(0) * mat_sizes.at(1));
        std::vector<float> mat2(mat_sizes.at(1) * mat_sizes.at(2));

        init(mat1);
        init(mat2);

        auto gpu_result = gpu_calc(mat1, mat2, mat_sizes);

        if (mat_sizes.at(0) * mat_sizes.at(1) * mat_sizes.at(2) < 1000) {
            auto cpu_result = control_calc(mat1, mat2, mat_sizes);
            std::println("\nVec1:");
            print(mat1, mat_sizes.at(0), mat_sizes.at(1));
            std::println("\nVec2:");
            print(mat2, mat_sizes.at(1), mat_sizes.at(2));
            std::println("\nProduct:");
            print(cpu_result, mat_sizes.at(0), mat_sizes.at(2));

            auto eq = is_equal(cpu_result, gpu_result);
            std::println("\nIs correct: {}", eq ? "true" : "false");
        }

    } catch (const std::exception &e) {
        std::println("{}", e.what());
        return -1;
    }
    return 0;
}
