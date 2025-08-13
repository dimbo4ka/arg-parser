#pragma once

#include <cstdint>
#include <string>
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
    virtual bool HasDefault() const = 0;
    virtual std::string GetDefault() const = 0;
    virtual std::size_t min_args() const = 0;
    virtual ArgumentType GetType() const = 0;
    virtual std::size_t GetArgCount() const = 0;
    virtual bool has_value() const = 0;
    virtual ~BaseArgument() = default;
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
    bool has_value() const override;
    std::size_t min_args() const override;
    std::size_t GetArgCount() const override;
    ArgumentType GetType() const override;
    bool HasDefault() const override;
    std::string GetDefault() const override;
    T GetValue(std::size_t index) const;

    void AddValue(T value);

private:
    std::vector<T> values_;
    std::vector<T>* stored_values_ = nullptr;
    T* stored_value_ = nullptr;
    std::size_t min_args_{};
    T default_value_;
    T value_{};
    bool is_positional_{};
    bool is_multivalue_{};
    bool has_default_value_{};
    bool has_value_{};
};

using StringArg = Argument<std::string>;
using FlagArg = Argument<bool>;
using IntArg = Argument<int32_t>;

template<typename T>
Argument<T>::Argument() 
        : stored_values_(nullptr), stored_value_(nullptr)
        , min_args_(0)
        , is_positional_(false), is_multivalue_(false)
        , has_default_value_(false) {}

template<typename T>
Argument<T>& Argument<T>::Default(T default_value) {
    default_value_ = std::move(default_value);
    has_default_value_ = true;
    return *this;
}
template<typename T>
Argument<T>& Argument<T>::MultiValue(std::size_t min_args) {
    is_multivalue_ = true;
    min_args_ = min_args;
    return *this;
}

template<typename T>
Argument<T>& Argument<T>::Positional() {
    is_positional_ = true;
    return *this;
}

template<typename T>
Argument<T>& Argument<T>::StoreValue(T& stored_value) {
    stored_value_ = &stored_value;
    return *this;
}

template<typename T>
Argument<T>& Argument<T>::StoreValues(std::vector<T>& stored_values) {
    if (!is_multivalue_) {
        throw std::logic_error("StoreValues can only be used with multi-value arguments");
    }
    stored_values_ = &stored_values;
    return *this;
}

template<typename T>
bool Argument<T>::IsPositional() const {
    return is_positional_;
}

template<typename T>
bool Argument<T>::IsMultiValue() const {
    return is_multivalue_;
}

template<typename T>
bool Argument<T>::has_value() const {
    return has_default_value_ || has_value_;
}

template<typename T>
std::size_t Argument<T>::min_args() const {
    return min_args_;
}

template<typename T>
std::size_t Argument<T>::GetArgCount() const {
    if (stored_values_)
        return stored_values_->size();
    return values_.size();
}

template<typename T>
ArgumentType Argument<T>::GetType() const {
    if constexpr (std::is_same_v<T, std::string>) {
        return ArgumentType::kString;
    } else if constexpr (std::is_same_v<T, int32_t>) {
        return ArgumentType::kInt;
    } else if constexpr (std::is_same_v<T, bool>) {
        return ArgumentType::kFlag;
    } else {
        throw std::runtime_error("Unsupported argument type");
    }
}

template<typename T>
bool Argument<T>::HasDefault() const {
    return has_default_value_;
}

template<typename T>
std::string Argument<T>::GetDefault() const {
    if (has_default_value_) {
        if constexpr (std::is_same_v<T, std::string>) {
            return default_value_;
        } else {
            return std::to_string(default_value_);
        }
    }
    return {};
}

template<typename T>
T Argument<T>::GetValue(std::size_t index) const {
    if (is_multivalue_) {
        if (stored_values_) {
            if (index >= stored_values_->size())
                throw std::out_of_range("Index out of range for multi-value argument");
            return (*stored_values_)[index];
        }
        if (index >= values_.size())
            throw std::out_of_range("Index out of range for multi-value argument");
        return values_[index];
    }
    if (index > 0)
        throw std::logic_error("Can only retrieve the first value of a single-value argument");

    if (stored_value_)
        return *stored_value_;
    if (has_default_value_ && !has_value_)
        return default_value_;
    return value_;
}

template<typename T>
void Argument<T>::AddValue(T value) {
    has_value_ = true;
    if (is_multivalue_) {
        if (stored_values_) {
            stored_values_->push_back(std::move(value));
            return;
        }
        values_.push_back(std::move(value));
        return;
    }
    if (stored_value_) {
        *stored_value_ = std::move(value);
        return;
    }
    value_ = std::move(value);
}

}