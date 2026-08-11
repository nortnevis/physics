#include "utils.hpp"

namespace ph {

std::mt19937_64 gen(std::random_device{}());
std::uniform_int_distribution<int> dist;
std::uniform_real_distribution<float> real_dist;

void seed(int s) {
    gen.seed(s);
    dist.reset();
}

int rand_int(int a, int b) {
    static int _a = dist.param().a();
    static int _b = dist.param().b();
    if (a != _a || b != _b) {
        using param_t = decltype(dist)::param_type;
        dist.param(param_t{a, b});
        _a = a;
        _b = b;
    }
    return dist(gen);
}

float rand_float(float a, float b) {
    static int _a = real_dist.param().a();
    static int _b = real_dist.param().b();
    if (a != _a || b != _b) {
        using param_t = decltype(real_dist)::param_type;
        real_dist.param(param_t{a, b});
        _a = a;
        _b = b;
    }
    return real_dist(gen);
}

bool is_number(std::string_view str) {
    if (str.empty()) {
        return false;
    }
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

int align(int x, int y) {
    // we use this formula to align x to y
    return (x + y - 1) / y * y;
}

std::tuple<cl::NDRange, cl::NDRange> get_task_ndranges(const cl::Device &dev, const std::vector<int> &sizes) {
    auto wg_max = dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    std::vector<size_t> locals;
    std::vector<size_t> globals;
    for (const auto &dim : sizes) {
        auto wg_size = dim >= wg_max ? wg_max : dim;
        locals.push_back(wg_size);
        globals.push_back(ph::align(dim, wg_size));
    }
    cl::NDRange local_range = {locals.at(0), locals.at(1), locals.at(2)};
    cl::NDRange global_range = {globals.at(0), globals.at(1), globals.at(2)};

    return std::make_tuple(local_range, global_range);
}

std::tuple<std::vector<size_t>, std::vector<size_t>> get_task_ranges(const cl::Device &dev,
                                                                     const std::vector<int> &sizes) {
    auto wg_max = dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    std::vector<size_t> locals;
    std::vector<size_t> globals;
    for (const auto &dim : sizes) {
        auto wg_size = dim >= wg_max ? wg_max : dim;
        locals.push_back(wg_size);
        globals.push_back(ph::align(dim, wg_size));
    }

    return std::make_tuple(locals, globals);
}

cl::Device get_deivce(cl::Context &context, Mode mode) {
    if (mode == Mode::GPU) {
        context = cl::Context(CL_DEVICE_TYPE_GPU);
    } else {
        context = cl::Context(CL_DEVICE_TYPE_CPU);
    }

    auto *err = new cl_int{0};
    auto dev_list = context.getInfo<CL_CONTEXT_DEVICES>(err);
    if (*err != CL_SUCCESS || dev_list.empty()) {
        throw std::runtime_error("No awailable compute device");
    }
    auto dev = dev_list.front();
    auto dev_name = dev.getInfo<CL_DEVICE_NAME>(err);
    if (*err != CL_SUCCESS) {
        throw std::runtime_error("Couldn't get device name");
    }
    auto dev_vendor = dev.getInfo<CL_DEVICE_VENDOR>(err);
    if (*err != CL_SUCCESS) {
        throw std::runtime_error("Couldn't get device's vendor");
    }

    std::println("Device name: {}", dev_name);
    std::println("Device's vendor: {}\n\n", dev_vendor);

    return dev;
}

cl::Program compile_kernel(const std::filesystem::path &path, cl::Context &context, cl::Device &dev) {
    std::ifstream cl_file(path);
    if (!cl_file.is_open()) {
        throw std::runtime_error(std::format("Failed to open specified OpenCL kernel file: {}", path.string()));
    }
    std::stringstream buffer;
    buffer << cl_file.rdbuf();
    auto str_view = buffer.view();

    cl::Program::Sources sources;
    sources.push_back({str_view.data(), str_view.size()});
    cl::Program program(context, sources);

    program.build({dev});
    cl_file.close();

    return program;
}

} // namespace ph
