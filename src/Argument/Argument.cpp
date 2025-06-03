#include "Argument.hpp"

namespace ArgumentParser {
template<typename T>
Argument<T>::Argument() 
        : stored_values_(nullptr), stored_value_(nullptr)
        , min_args_(0)
        , is_positional_(false), is_multivalue_(false) {}

template<typename T>
Argument<T>& Argument<T>::Default(T) {
    default_value_ = std::move(T);
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
}
