int main(int, char **) {
    cl::Context ctx;
    auto dev = ph::get_deivce(ctx);

    auto wg_size = dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    auto wi_count = dev.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

    std::println("Max work group size: {}", wg_size);
    std::println("Max work items number: {}\n", wi_count);

    std::vector init{240, 618, 1030};
    auto [local, global] = ph::get_task_ranges(dev, init);

    std::println("Initial sizes: {}", init);
    std::println("Local ranges: {}", local);
    std::println("Global ranges: {}", global);

    return 0;
}
