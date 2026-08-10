int main(int, char **) {
    cl::Context ctx;
    auto dev = ph::get_deivce(ctx);

    auto wg_size = dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    auto wi_count = dev.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

    std::println("Max work group size: {}", wg_size);
    std::println("Max work items number: {}", wi_count);

    return 0;
}
