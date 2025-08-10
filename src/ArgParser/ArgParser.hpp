#pragma once

#include <cstdint>
#include <string>

#include "src/Argument/Argument.hpp"

namespace ArgumentParser {
class ArgParser {
public:
    ArgParser(const std::string&);
    ArgParser(std::string&&);
    
    bool Parse(const std::vector<std::string>&);
    bool Parse(std::vector<std::string>&&);

    StringArg& AddStringArgument(char, const std::string&, const std::string = "");
    StringArg& AddStringArgument(const std::string&, const std::string = "");
    StringArg& AddStringArgument(std::string&&, const std::string = "");
    std::string GetStringValue(const std::string&, std::size_t = 0);

    IntArg& AddIntArgument(char, const std::string&, const std::string = "");
    IntArg& AddIntArgument(const std::string&, const std::string = "");
    IntArg& AddIntArgument(std::string&&, const std::string = "");
    int32_t GetIntValue(const std::string&, std::size_t = 0);

    FlagArg& AddFlag(char, const std::string&, const std::string = "");
    FlagArg& AddFlag(const std::string&, const std::string = "");
    FlagArg& AddFlag(std::string&&, const std::string = "");
    bool GetFlag(const std::string&, std::size_t = 0);

    void AddHelp(char, const std::string&, const std::string = "");

    bool Help() const;
    std::string HelpDescription() const;

private:

};
};