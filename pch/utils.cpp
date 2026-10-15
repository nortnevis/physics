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
    static float _a = real_dist.param().a();
    static float _b = real_dist.param().b();
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

std::expected<cl::Device, DeviceError> get_device(cl::Context &context, Mode mode) {
    if (mode == Mode::GPU) {
        context = cl::Context(CL_DEVICE_TYPE_GPU);
    } else {
        context = cl::Context(CL_DEVICE_TYPE_CPU);
    }

    auto *err = new cl_int{0};
    auto dev_list = context.getInfo<CL_CONTEXT_DEVICES>(err);
    if (*err != CL_SUCCESS || dev_list.empty()) {
        return std::unexpected(DeviceError::ComputeDeviceIsNotAwailable);
    }
    auto dev = dev_list.front();
    auto dev_name = dev.getInfo<CL_DEVICE_NAME>(err);
    if (*err != CL_SUCCESS) {
        return std::unexpected(DeviceError::DeviceNameIsNotAwailable);
    }
    auto dev_vendor = dev.getInfo<CL_DEVICE_VENDOR>(err);
    if (*err != CL_SUCCESS) {
        return std::unexpected(DeviceError::DeviceVendorIsNotAwailable);
    }

    std::println("Device name: {}", dev_name);
    std::println("Device's vendor: {}\n", dev_vendor);

    return dev;
}

std::expected<cl::Program, KernelError> compile_kernel(const std::filesystem::path &path, cl::Context &context,
                                                       cl::Device &dev) {
    std::ifstream cl_file(path);
    if (!cl_file.is_open()) {
        return std::unexpected(KernelError::FailedToOpenKernelCodeTextFile);
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
