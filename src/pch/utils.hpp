#pragma once

namespace ph {

void seed(int s);

int rand_int(int a, int b);
float rand_float(float a, float b);

bool is_number(std::string_view str);

int align(int x, int y);

template <typename Container>
std::tuple<cl::NDRange, cl::NDRange> get_task_ndranges(const cl::Device &dev, const Container &sizes) {
    auto wg_max = dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    std::vector<size_t> locals;
    std::vector<size_t> globals;
    for (const auto &dim : sizes) {
        auto wg_size = std::min(static_cast<std::remove_cvref_t<decltype(wg_max)>>(dim), wg_max);
        locals.push_back(wg_size);
        globals.push_back(ph::align(dim, wg_size));
    }
    cl::NDRange local_range;
    cl::NDRange global_range;
    if (sizes.size() == 1) {
        local_range = {locals.at(0)};
        global_range = {globals.at(0)};
    }
    if (sizes.size() == 2) {
        local_range = {locals.at(0), locals.at(1)};
        global_range = {globals.at(0), globals.at(1)};
    }
    if (sizes.size() == 3) {
        local_range = {locals.at(0), locals.at(1), locals.at(2)};
        global_range = {globals.at(0), globals.at(1), globals.at(2)};
    }

    return std::make_tuple(local_range, global_range);
}

template <typename Container>
std::tuple<std::vector<size_t>, std::vector<size_t>> get_task_ranges(const cl::Device &dev, const Container &sizes) {
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

enum class Mode {
    CPU,
    GPU,
    _count,
};

cl::Device get_deivce(cl::Context &context, Mode mode = Mode::GPU);

cl::Program compile_kernel(const std::filesystem::path &path, cl::Context &context, cl::Device &dev);

} // namespace ph
