#pragma once

extern ph::Mode mode;

std::vector<int> parse_args(int argc, const char *argv[]);

void init(std::vector<float> &mat);

void print(const std::vector<float> &mat, int rows, int cols);

bool is_equal(std::vector<float> &l, std::vector<float> &r);

std::vector<float> transpose(const std::vector<float> &mat, int rows, int cols);
