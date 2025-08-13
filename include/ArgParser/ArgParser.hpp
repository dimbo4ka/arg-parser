#pragma once

#include <charconv>
#include <cstdint>
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
    std::string GetStringValue(const std::string&, std::size_t = 0);

    IntArg& AddIntArgument(char, std::string, std::string = "");
    IntArg& AddIntArgument(std::string, std::string = "");
    int32_t GetIntValue(const std::string&, std::size_t = 0);

    FlagArg& AddFlag(char, std::string, std::string = "");
    FlagArg& AddFlag(std::string, std::string = "");
    bool GetFlag(const std::string&, std::size_t = 0);

    void AddHelp(char, std::string, std::string = "");

    bool Help() const;
    std::string HelpDescription() const;

private:
    std::vector<std::unique_ptr<BaseArgument>> arguments_;
    
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

    char GetShortName(const std::string& full_name) const;
    std::unique_ptr<BaseArgument>& GetArgument(const std::string& full_name);
    const std::unique_ptr<BaseArgument>& GetArgument(const std::string& full_name) const;

    bool ParseShortArgument(const std::vector<std::string>& args, size_t& i);
    bool ParseFullArgument(const std::vector<std::string>& args, size_t& i);
    bool ParsePositionalArgument(const std::vector<std::string>& args, size_t& i);
    bool ParseShortFlags(const std::string& args);
    bool SetPositionalArgument();

    bool IsCorrectMultiValue() const;
    bool AllHaveValues() const;

    bool ConvertToNumber(const char* str, int32_t& number);
};
};