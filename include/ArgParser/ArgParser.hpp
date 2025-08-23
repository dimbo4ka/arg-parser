#pragma once

#include <charconv>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

#include "Argument/Argument.hpp"

namespace ArgumentParser {
class ArgParser {
public:
    ArgParser(std::string);
    
    bool Parse(const std::vector<std::string>&);
    bool Parse(int argc, char** argv);

    StringArg& AddStringArgument(char, std::string, std::string = "");
    StringArg& AddStringArgument(std::string, std::string = "");
    std::string GetStringValue(const std::string&, std::size_t = 0) const;

    IntArg& AddIntArgument(char, std::string, std::string = "");
    IntArg& AddIntArgument(std::string, std::string = "");
    int64_t GetIntValue(const std::string&, std::size_t = 0) const;

    FlagArg& AddFlag(char, std::string, std::string = "");
    FlagArg& AddFlag(std::string, std::string = "");
    bool GetFlag(const std::string&, std::size_t = 0) const;

    void AddHelp(char, std::string, std::string = "");

    bool Help() const;
    std::string HelpDescription() const;

private:
    std::vector<std::unique_ptr<BaseArgument>> arguments_;
    using ArgPtr = std::unique_ptr<BaseArgument>;
    
    std::string parser_name_;

    std::unordered_map<std::string, char> short_names_;
    std::unordered_map<char, std::string> full_names_;
    std::unordered_map<std::string, std::string> descriptions_;
    std::unordered_map<std::string, std::size_t> argument_indices_;

    std::string full_help_name_;
    std::string help_description_;
    char short_help_name_;
    bool has_help_;

    std::string positional_argument_name_;

    inline static const std::unordered_map<ArgumentType, std::string> kTypeNames {
        {ArgumentType::kString, "string"},
        {ArgumentType::kInt, "int"},
        {ArgumentType::kFlag, "flag"},
    };

    struct ParseContext {
        const std::vector<std::string>& args;
        const std::string& full_name;
        std::size_t& index;
        std::size_t equal_pos;
        bool is_named;
    };

    enum class ValidationResult {
        kValid,
        kHandled,
        kError
    };

    void SetNames(char short_name, const std::string& full_name);
    void SetDescription(std::string& full_name, std::string& description);
    void SetArgument(char short_name, std::string& full_name, std::string& description);
    void CheckShortName(char short_name) const;

    template<typename T>
    T& AddArgument();
    
    template<typename T>
    T& GetArgumentAs(const std::string& full_name, ArgumentType expected_type) const;

    char GetShortName(const std::string& full_name) const;
    ArgPtr& GetArgument(const std::string& full_name);
    const ArgPtr& GetArgument(const std::string& full_name) const;

    bool ParseArgument(ParseContext&);
    bool ParseTypeArgument(ParseContext&, bool = false);
    bool ParseMultivalue(ParseContext&);
    bool ParseStringArgument(ParseContext&);
    bool ParseIntArgument(ParseContext&);
    bool ParseFlagArgument(ParseContext&);
    bool NextValueExists(ParseContext&) const;
    
    bool ParseShortArgument(const std::vector<std::string>& args, size_t& i);
    ValidationResult ValidateShortName(char short_name);
    ValidationResult ValidateShortArgument(const std::string& arg);
    bool ParseShortFlags(const std::string& args);
    bool ParseFullArgument(const std::vector<std::string>& args, size_t& i);

    bool ParsePositionalArgument(const std::vector<std::string>& args, size_t& i);
    bool ParsePositionalStringArgument(ArgPtr& argument, const std::string& str);
    bool ParsePositionalIntArgument(ArgPtr& argument, const std::string& str);
    bool ParsePositionalFlagArgument();

    bool IsNamedArgument(const std::string& arg) const;
    bool SetPositionalArgument();

    bool IsCorrectMultiValue() const;
    bool AllHaveValues() const;

    bool ConvertToNumber(const char* str, int64_t& number);
};

template<typename T>
T& ArgParser::AddArgument() {
    std::unique_ptr<T> arg_ptr = std::make_unique<T>();
    T& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));
    return arg;
}

template<typename T>
T& ArgParser::GetArgumentAs(const std::string& full_name, ArgumentType expected_type) const {
    const ArgPtr& arg = GetArgument(full_name);
    if (arg->GetType() != expected_type)
        throw std::invalid_argument(std::format("Argument is not of type {}", kTypeNames.at(expected_type)));
    return dynamic_cast<T&>(*arg);
}
}