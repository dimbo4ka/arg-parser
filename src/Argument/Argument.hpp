#pragma once

#include <cstdint>
#include <vector>

namespace ArgumentParser {
enum class ArgumentType {
    kString,
    kInt,
    kFlag
};

class BaseArgument {
public:
    virtual bool IsPositional() const = 0; 
    virtual bool IsMultiValue() const = 0;
    virtual std::size_t min_args() const = 0;
    virtual ArgumentType GetType() const = 0;
};

template<typename T>
class Argument : public BaseArgument {
public:
    Argument();
    Argument& Default(T);
    Argument& MultiValue(std::size_t = 0);
    Argument& Positional();
    Argument& StoreValue(T&);
    Argument& StoreValues(std::vector<T>&);

    bool IsPositional() const override; 
    bool IsMultiValue() const override;
    std::size_t min_args() const override;
    ArgumentType GetType() const override;
    T GetValue(std::size_t index) const;

private:
    std::vector<T>* stored_values_;
    T* stored_value_;
    std::size_t min_args_;
    T default_value_;
    T value_;
    bool is_positional_;
    bool is_multivalue_;
    bool has_default_value_;
};

using StringArg = Argument<std::string>;
using FlagArg = Argument<bool>;
using IntArg = Argument<int32_t>;
}