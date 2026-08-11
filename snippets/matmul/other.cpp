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
            std::print("{} ", mat[i * rows + j]);
        }
        std::println("");
    }
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
            tns.push_back(mat[i * rows + j]);
        }
    }
    return tns;
}
