#pragma once

#include <cstdint>
#include <vector>

namespace ArgumentParser {
class BaseArgument {};

template<typename T>
class Argument : public BaseArgument {
public:
    Argument();
    Argument& Default(T);
    Argument& MultiValue(std::size_t = 0);
    Argument& Positional();
    Argument& StoreValue(T&);
    Argument& StoreValues(std::vector<T>&);

private:
    std::vector<T>* stored_values_;
    T* stored_value_;
    std::size_t min_args_;
    T default_value_;
    bool is_positional_;
    bool is_multivalue_;
};

using StringArg = Argument<std::string>;
using FlagArg = Argument<bool>;
using IntArg = Argument<int32_t>;
};
