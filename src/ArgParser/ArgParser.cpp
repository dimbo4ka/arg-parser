#include "ArgParser/ArgParser.hpp"

#include <iostream>

namespace ArgumentParser {
ArgParser::ArgParser(std::string parser_name) 
        : parser_name_(std::move(parser_name))
        , short_help_name_('\0')
        , has_help_(false) {}

StringArg& ArgParser::AddStringArgument(
        char short_name, 
        std::string full_name, std::string description) {
    if (!isalpha(short_name))
        throw std::invalid_argument("Short name must be a single alphabetic character");

    short_names_[full_name] = short_name;
    full_names_[short_name] = full_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<StringArg> arg_ptr = std::make_unique<StringArg>();
    StringArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));
    return arg;
}

StringArg& ArgParser::AddStringArgument(std::string full_name, 
                                        std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<StringArg> arg_ptr = std::make_unique<StringArg>();
    StringArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));
    return arg;
}

std::string ArgParser::GetStringValue(const std::string& full_name, 
                                      std::size_t index) {  
    if (!argument_indices_.contains(full_name))
        throw std::invalid_argument("Unknown argument: " + full_name);
    
    std::size_t arg_index = argument_indices_[full_name];
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kString) {
        throw std::invalid_argument("Argument is not of string type");
    }
    return dynamic_cast<StringArg&>(*arg).GetValue(index);
}

IntArg& ArgParser::AddIntArgument(char short_name, std::string full_name, 
                                  std::string description) {
    if (!isalpha(short_name))
        throw std::invalid_argument("Short name must be a single alphabetic character");

    short_names_[full_name] = short_name;
    full_names_[short_name] = full_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<IntArg> arg_ptr = std::make_unique<IntArg>();
    IntArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));   
    return arg;
}

IntArg& ArgParser::AddIntArgument(std::string full_name, 
                                  std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<IntArg> arg_ptr = std::make_unique<IntArg>();
    IntArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));       
    return arg;
}

int32_t ArgParser::GetIntValue(const std::string& full_name,    
                               std::size_t index) {
    if (!argument_indices_.contains(full_name))
        throw std::invalid_argument("Unknown argument: " + full_name);

    std::size_t arg_index = argument_indices_[full_name];
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kInt) {
        throw std::invalid_argument("Argument is not of int type");
    }
    return dynamic_cast<IntArg&>(*arg).GetValue(index);
}

FlagArg& ArgParser::AddFlag(char short_name, std::string full_name, 
                            std::string description) {
    if (!isalpha(short_name))
        throw std::invalid_argument("Short name must be a single alphabetic character");
    
    short_names_[full_name] = short_name;
    full_names_[short_name] = full_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<FlagArg> arg_ptr = std::make_unique<FlagArg>();
    FlagArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));   
    return arg;
}

FlagArg& ArgParser::AddFlag(std::string full_name, std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<FlagArg> arg_ptr = std::make_unique<FlagArg>();
    FlagArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::move(arg_ptr));
    return arg;
}

bool ArgParser::GetFlag(const std::string& full_name, std::size_t index) {
    if (!argument_indices_.contains(full_name))
        throw std::invalid_argument("Unknown argument: " + full_name);
    
    std::size_t arg_index = argument_indices_[full_name];
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kFlag) {
        throw std::invalid_argument("Argument is not of flag type");
    }
    return dynamic_cast<FlagArg&>(*arg).GetValue(index);
}

void ArgParser::AddHelp(char short_name, std::string full_name, 
                        std::string description) {
    if (!isalpha(short_name))
        throw std::invalid_argument("Short name must be a single alphabetic character");

    short_help_name_ = short_name;
    full_help_name_ = std::move(full_name);
    help_description_ = std::move(description);
}

bool ArgParser::Help() const {
    return has_help_;
}

std::string ArgParser::HelpDescription() const {
    return help_description_;
}

bool ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);
    return Parse(args);
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        if (Help())
            return true;
        const std::string& arg = args[i];
        if (arg.starts_with("--")) {
            if (!ParseFullArgument(args, i))
                return false;
        } else if (arg.starts_with('-')) {
            if (!ParseShortArgument(args, i))
                return false;
        } else {
            if (!ParsePositionalArgument(args, i))
                return false;
        }
    }

    return Help() || IsCorrectMultiValue() && AllHaveValues();
}

bool ArgParser::ParseShortArgument(const std::vector<std::string>& args, size_t& i) {
    const std::string& arg = args[i];
    char short_name = arg[1];
    auto equal_pos = arg.find('='); 
    bool is_named = (equal_pos != std::string::npos);
    if (is_named && equal_pos < 2) {
        std::cerr << "Invalid short argument format: " << arg << std::endl;
        return false;
    }
    if (!is_named && arg.size() > 2) 
        return ParseShortFlags(arg);
    if (is_named && equal_pos != 2) {
        std::cerr << "Invalid short argument format: " << arg << std::endl;
        return false;
    }

    if (!full_names_.contains(short_name)) {
        std::cerr << "Unknown short argument: " << short_name << std::endl;
        return false;
    }

    std::string full_name = full_names_[short_name];
    auto& argument = arguments_[argument_indices_[full_name]];
    switch (argument->GetType()) {
        case ArgumentType::kString: {
            if (!is_named && i + 1 >= args.size()) {
                std::cerr << "Expected string value for argument: " << full_name << std::endl;
                return false;
            }
            std::string value;
            if (is_named) {
                value = std::move(arg.substr(equal_pos + 1));
            } else {
                value = args[++i];
            }
            dynamic_cast<StringArg&>(*argument).AddValue(std::move(value));
            break;
        }
        case ArgumentType::kInt: {
            if (!is_named && i + 1 >= args.size()) {
                std::cerr << "Expected value for argument: " << full_name << std::endl;
                return false;
            }
            int32_t number;
            bool has_parsing_error = is_named && !ConvertToNumber(args[i].c_str() + equal_pos + 1, number)
                                  || !is_named && !ConvertToNumber(args[++i].c_str(), number);
            if (has_parsing_error) {
                std::cerr << "Invalid integer value for argument: " << full_name << std::endl;
                return false;
            }
            dynamic_cast<IntArg&>(*argument).AddValue(number);
            break;
        }
        case ArgumentType::kFlag: {
            if (is_named) {
                std::cerr << "Flag can not be named argument: " << full_name << std::endl;
                return false;
            }
            dynamic_cast<FlagArg&>(*argument).AddValue(true);
        }
    }

    return true;
}

bool ArgParser::ParseFullArgument(const std::vector<std::string>& args, size_t& i) {
    const std::string& arg = args[i];
    std::string full_name;
    auto equal_pos = arg.find('='); 
    bool is_named = (equal_pos != std::string::npos);

    if (is_named) {
        full_name = arg.substr(2, equal_pos - 2);
    } else {
        full_name = arg.substr(2);
        if (full_name == full_help_name_) {
            has_help_ = true;
            return true;
        }
    }

    if (!argument_indices_.contains(full_name)) {
        std::cerr << "Unknown argument: " << full_name << std::endl;
        return false;
    }
    auto& argument = arguments_[argument_indices_[full_name]];
    switch (argument->GetType()) {
        case ArgumentType::kString: {
            if (i + 1 >= args.size() && !is_named) {
                std::cerr << "Expected string value for argument: " << full_name << std::endl;
                return false;
            }
            if (is_named) {
                dynamic_cast<StringArg&>(*argument).AddValue(args[i].substr(equal_pos + 1));
            } else {
                dynamic_cast<StringArg&>(*argument).AddValue(args[++i]);
            }
            break;
        }
        case ArgumentType::kInt: {
            if (i + 1 >= args.size() && !is_named) {
                std::cerr << "Expected integer value for argument: " << full_name << std::endl;
            }
            int32_t number;
            bool has_parsing_error = is_named && !ConvertToNumber(args[i].c_str() + equal_pos + 1, number)
                                  || !is_named && !ConvertToNumber(args[++i].c_str(), number);  
            if (has_parsing_error) {
                std::cerr << "Invalid integer value for argument: " << full_name << std::endl;
                return false;
            }
            dynamic_cast<IntArg&>(*argument).AddValue(number);
            break;
        }
        case ArgumentType::kFlag: {
            if (is_named) {
                std::cerr << "Flag can not be named argument: " << full_name << std::endl;
                return false;
            }
            dynamic_cast<FlagArg&>(*argument).AddValue(true);
            break;
        } default: {
            break;
        }
    }
    return true;
}

bool ArgParser::ParsePositionalArgument(const std::vector<std::string>& args, size_t& i) {
    const std::string& arg = args[i];
    if (positional_argument_name_.empty()) {
        if (!SetPositionalArgument()) {
            std::cerr << "No positional argument defined." << std::endl;
            return false;
        }
    }
    size_t arg_index = argument_indices_[positional_argument_name_];
    std::unique_ptr<BaseArgument>& argument = arguments_[arg_index];
    switch(argument->GetType()) {
        case ArgumentType::kString: {
            dynamic_cast<Argument<std::string>&>(*argument).AddValue(arg);
            break;
        }
        case ArgumentType::kInt: {
            int32_t number;
            if (!ConvertToNumber(arg.c_str(), number)) {
                std::cerr << "Invalid integer value for argument: " << positional_argument_name_ << std::endl;
                return false;
            }
            dynamic_cast<Argument<int32_t>&>(*argument).AddValue(number);
            break;
        }
        default: {
            std::cerr << "Flag is not supported for positional arguments." << std::endl;
            return false;
        }
    }
    return true;
}

bool ArgParser::SetPositionalArgument() {
    for (auto& [full_name, index] : argument_indices_) {
        if (arguments_[index]->IsPositional()) {
            positional_argument_name_ = full_name;
            return true;
        }
    }
    return false;
}

bool ArgParser::IsCorrectMultiValue() const {
    for (auto& argument : arguments_) {
        if (argument->IsMultiValue() && argument->min_args() > argument->GetArgCount()) {
            std::cout << "Multi-value argument" << "has incorrect number of values." << std::endl;
            return false;
        }
    }
    return true;
}

bool ArgParser::AllHaveValues() const {
    for (const auto& argument : arguments_) {
        if (!argument->has_value() && !argument->IsPositional() 
                                   && argument->GetType() != ArgumentType::kFlag) {
            return false;
        }
    }
    return true;
}

bool ArgParser::ParseShortFlags(const std::string& arg) {
    for (size_t i = 1; i < arg.size(); ++i) {
        char flag = arg[i];
        if (flag == short_help_name_) {
            has_help_ = true;
            return true;
        }
        if (!full_names_.contains(flag)) {
            std::cerr << "Unknown flag: " << flag << std::endl;
            return false;
        }
        std::string full_name = full_names_[flag];
        dynamic_cast<FlagArg&>(*arguments_[argument_indices_[full_name]]).AddValue(true);
    }
    return true;
}

bool ArgParser::ConvertToNumber(const char* str, int32_t& number) {
    auto [_, ec] = std::from_chars(str, str + strlen(str), number);
    return ec == std::errc{};
}

}