#include "other.hpp"

ph::Mode mode = ph::Mode::GPU;

std::vector<int> parse_args(int argc, const char *argv[]) {
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
            std::print("{} ", mat[i * cols + j]);
        }
        std::println("");
    }
}

void save2file(const std::vector<float> &mat, int rows, int cols, std::filesystem::path path) {
    assert(rows * cols == mat.size() && "rows*cols != mat.size()");
    static_assert(sizeof(float) == 4 && "sizeof(float) != 4");
    if (path.empty()) {
        auto time_stamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
        path = std::format("{0:%F}-{0:%H}-{0:%M}-{0:%S}.dot", time_stamp);
    }
    if (std::filesystem::exists(path)) {
        for (int _ = 0; _ < 3; ++_) {
            auto time_stamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
            std::string new_path = std::format("{0:%F}-{0:%H}-{0:%M}-{0:%S}.dot", time_stamp);
            if (!std::filesystem::exists(new_path)) {
                path = new_path;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
            } else if (_ == 2) {
                throw std::system_error(std::make_error_code(std::errc::file_exists),
                                        std::format("Can't save to '{}' file.", new_path).c_str());
            }
        }
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::system_error(std::make_error_code(std::errc::bad_file_descriptor),
                                std::format("Can't open for write file '{}'.", path.string()).c_str());
    }
    size_t buff_size = 64 * 1024;
    char *buff = new char[buff_size]{0};
    file.rdbuf()->pubsetbuf(buff, buff_size);

    file << (size_t)(rows) << (size_t)(cols);

    file.write(reinterpret_cast<const char *>(mat.data()),
               mat.size() * sizeof(std::decay_t<decltype(mat)>::value_type));
    file.close();
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

std::vector<float> transpose(const std::vector<float> &mat, int rows, int cols) {
    std::vector<float> tns;
    tns.reserve(mat.size());
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < rows; ++i) {
            tns.push_back(mat[i * cols + j]);
        }
    }
    return tns;
}
