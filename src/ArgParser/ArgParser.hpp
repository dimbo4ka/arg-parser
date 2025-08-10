#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "src/Argument/Argument.hpp"

namespace ArgumentParser {
class ArgParser {
public:
    ArgParser(std::string);
    
    bool Parse(std::vector<std::string>);

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
    std::unordered_map<std::string, std::string> descriptions_;
    std::unordered_map<std::string, std::size_t> argument_indices_;

    std::string full_help_name_;
    std::string help_description_;
    char short_help_name_;
};
};