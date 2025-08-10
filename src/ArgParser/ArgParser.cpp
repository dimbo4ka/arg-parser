#include "ArgParser.hpp"

#include <memory>

namespace ArgumentParser {
ArgParser::ArgParser(std::string parser_name) 
        : parser_name_(std::move(parser_name))
        ,  short_help_name_('\0') {}

StringArg& ArgParser::AddStringArgument(
        char short_name, 
        std::string full_name, std::string description) {
    short_names_[full_name] = short_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<StringArg> arg_ptr = std::make_unique<StringArg>();
    StringArg& arg = *arg_ptr; 
    arguments_.emplace_back(std::make_unique<StringArg>());
    return arg;
}

StringArg& ArgParser::AddStringArgument(std::string full_name, 
                                        std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<StringArg> arg_ptr = std::make_unique<StringArg>();
    StringArg& arg = *arg_ptr; 
    arguments_.emplace_back(arg_ptr);
    return arg;
}

std::string ArgParser::GetStringValue(const std::string& full_name, 
                                      std::size_t index) {  
    std::size_t arg_index = argument_indices_.at(full_name);
    if (arg_index >= arguments_.size()) {
        throw std::out_of_range("Invalid argument index");
    }
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kString) {
        throw std::invalid_argument("Argument is not of string type");
    }
    return dynamic_cast<StringArg&>(*arg).GetValue(index);
}

IntArg& ArgParser::AddIntArgument(char short_name, std::string full_name, 
                                  std::string description) {
    short_names_[full_name] = short_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<IntArg> arg_ptr = std::make_unique<IntArg>();
    IntArg& arg = *arg_ptr; 
    arguments_.emplace_back(arg_ptr);   
    return arg;
}

IntArg& ArgParser::AddIntArgument(std::string full_name, 
                                  std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<IntArg> arg_ptr = std::make_unique<IntArg>();
    IntArg& arg = *arg_ptr; 
    arguments_.emplace_back(arg_ptr);       
    return arg;
}

int32_t ArgParser::GetIntValue(const std::string& full_name,    
                               std::size_t index) {
    std::size_t arg_index = argument_indices_.at(full_name);
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kInt) {
        throw std::invalid_argument("Argument is not of int type");
    }
    return dynamic_cast<IntArg&>(*arg).GetValue(index);
}

FlagArg& ArgParser::AddFlag(char short_name, std::string full_name, 
                            std::string description) {
    short_names_[full_name] = short_name;
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<FlagArg> arg_ptr = std::make_unique<FlagArg>();
    FlagArg& arg = *arg_ptr; 
    arguments_.emplace_back(arg_ptr);   
    return arg;
}

FlagArg& ArgParser::AddFlag(std::string full_name, std::string description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();

    std::unique_ptr<FlagArg> arg_ptr = std::make_unique<FlagArg>();
    FlagArg& arg = *arg_ptr; 
    arguments_.emplace_back(arg_ptr);
    return arg;
}

bool ArgParser::GetFlag(const std::string& full_name, std::size_t index) {
    std::size_t arg_index = argument_indices_.at(full_name);
    std::unique_ptr<BaseArgument>& arg = arguments_[arg_index];
    if (arg->GetType() != ArgumentType::kFlag) {
        throw std::invalid_argument("Argument is not of flag type");
    }
    return dynamic_cast<FlagArg&>(*arg).GetValue(index);
}

void ArgParser::AddHelp(char short_name, std::string full_name, 
                        std::string description) {
    short_help_name_ = short_name;
    full_help_name_ = std::move(full_name);
    help_description_ = std::move(description);
}

bool ArgParser::Help() const {
    return short_help_name_ != '\0';
}

std::string ArgParser::HelpDescription() const {
    return help_description_;
}
};
