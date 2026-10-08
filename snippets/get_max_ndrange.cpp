int main(int, char **) {
    cl::Context ctx;
    auto dev_result = ph::get_device(ctx);
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
            std::println("Unknown error on get_device");
            break;
        }
        std::terminate();
    }
    auto &dev = *dev_result;
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
