#include "Argument.hpp"

namespace ArgumentParser {
template<typename T>
Argument<T>::Argument() 
        : stored_values_(nullptr), stored_value_(nullptr)
        , min_args_(0)
        , is_positional_(false), is_multivalue_(false)
        , has_default_value_(false) {}

template<typename T>
Argument<T>& Argument<T>::Default(T) {
    default_value_ = std::move(T);
    has_default_value_ = true;
}
template<typename T>
Argument<T>& Argument<T>::MultiValue(std::size_t min_args) {
    is_multivalue_ = true;
    min_args_ = min_args;
}

template<typename T>
Argument<T>& Argument<T>::Positional() {
    is_positional_ = true;
}

template<typename T>
Argument<T>& Argument<T>::StoreValue(T& stored_value) {
    stored_value_ = &stored_value;
}

template<typename T>
Argument<T>& Argument<T>::StoreValues(std::vector<T>& stored_values) {
    stored_values_ = &stored_values;
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
std::size_t Argument<T>::min_args() const {
    return min_args_;
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
T Argument<T>::GetValue(std::size_t index) const {
    return value_;
}

}
