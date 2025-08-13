#include "ArgParser/ArgParser.hpp"

#include <cstring>
#include <iostream>

namespace ArgumentParser {
ArgParser::ArgParser(std::string parser_name) 
        : parser_name_(std::move(parser_name))
        , short_help_name_('\0')
        , has_help_(false) {}

StringArg& ArgParser::AddStringArgument(char short_name, std::string full_name, std::string description) {
    CheckShortName(short_name);
    SetArgument(short_name, full_name, description);
    return AddArgument<StringArg>();
}

StringArg& ArgParser::AddStringArgument(std::string full_name, std::string description) {
    SetDescription(full_name, description);
    return AddArgument<StringArg>();
}

std::string ArgParser::GetStringValue(const std::string& full_name, std::size_t index) const {  
    return GetArgumentAs<StringArg>(full_name, ArgumentType::kString).GetValue(index);
}

IntArg& ArgParser::AddIntArgument(char short_name, std::string full_name, std::string description) {
    CheckShortName(short_name);
    SetArgument(short_name, full_name, description);
    return AddArgument<IntArg>();
}

IntArg& ArgParser::AddIntArgument(std::string full_name, std::string description) {
    SetDescription(full_name, description);
    return AddArgument<IntArg>();
}

int64_t ArgParser::GetIntValue(const std::string& full_name, std::size_t index) const {
    return GetArgumentAs<IntArg>(full_name, ArgumentType::kInt).GetValue(index);
}

FlagArg& ArgParser::AddFlag(char short_name, std::string full_name, std::string description) {
    CheckShortName(short_name);
    SetArgument(short_name, full_name, description);
    return AddArgument<FlagArg>();
}

FlagArg& ArgParser::AddFlag(std::string full_name, std::string description) {
    SetDescription(full_name, description);
    return AddArgument<FlagArg>();
}

bool ArgParser::GetFlag(const std::string& full_name, std::size_t index) const {
    return GetArgumentAs<FlagArg>(full_name, ArgumentType::kFlag).GetValue(index);
}

void ArgParser::AddHelp(char short_name, std::string full_name, std::string description) {
    CheckShortName(short_name);
    short_help_name_ = short_name;
    full_help_name_ = std::move(full_name);
    help_description_ = std::move(description);
}

bool ArgParser::Help() const {
    return has_help_;
}

std::string ArgParser::HelpDescription() const {
    std::string help_description 
                        = parser_name_ + "\n" 
                        + help_description_ + "\n\n";

    for (const auto& [full_name, description] : descriptions_) {
        std::string short_name;
        if (short_names_.contains(full_name)) {
            char symbol = GetShortName(full_name);
            short_name = "-" + std::string(1, symbol) + ",  ";
        } else {
            short_name = "     ";
        }
        std::string long_name = "--" + full_name;
        const auto& arg = GetArgument(full_name);
        if (arg->GetType() == ArgumentType::kString) {
            long_name += "=<string>";
        } else if (arg->GetType() == ArgumentType::kInt) {
            long_name += "=<int>";
        }
        long_name += ",  ";

        std::string full_description = description + (description.empty() ? "" : " ");
        if (arg->IsMultiValue() || arg->IsPositional() || arg->HasDefault()) {
            full_description += "[";
            bool need_comma = false;
            if (arg->IsMultiValue()) {
                full_description += "repeated, min args = " + std::to_string(arg->min_args());
                need_comma = true;
            }
            if (arg->IsPositional()) {
                if (need_comma) 
                    full_description += ", ";
                full_description += "positional";
            }
            if (arg->HasDefault()) {
                if (need_comma) 
                    full_description += ", ";
                full_description += "default = " + arg->GetDefault();
            }
            full_description += "]";
        }

        help_description += short_name + long_name + full_description + "\n";
    }
    help_description += "\n";
    std::string help_symbol = std::string(1, short_help_name_);
    help_description += "-" + help_symbol 
                     + ",  --" + full_help_name_ 
                     + " Display this help and exit\n";
    return help_description;
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

    return Help() || (IsCorrectMultiValue() && AllHaveValues());
}

bool ArgParser::ParseShortArgument(const std::vector<std::string>& args, size_t& i) {
    const std::string& arg = args[i];    
    ValidationResult validation_result = ValidateShortArgument(arg); 
    if (validation_result == ValidationResult::kHandled)
        return true;
    if (validation_result == ValidationResult::kError)
        return false;
    
    char short_name = arg[1];
    validation_result = ValidateShortName(short_name);
    if (validation_result == ValidationResult::kHandled)
        return true;
    if (validation_result == ValidationResult::kError)
        return false;
    
    std::string full_name = full_names_[short_name];
    ParseContext context{args, full_name, i, arg.find('='), IsNamedArgument(arg)};
    return ParseArgument(context);
}

ArgParser::ValidationResult ArgParser::ValidateShortName(char short_name) {
    if (!full_names_.contains(short_name)) {
        if (short_name == short_help_name_) {
            has_help_ = true;
            return ValidationResult::kHandled;
        }
        std::cerr << "Unknown short argument: " << short_name << std::endl;
        return ValidationResult::kError;
    }
    return ValidationResult::kValid;
}

auto ArgParser::ValidateShortArgument(const std::string& arg) 
        -> ArgParser::ValidationResult {
    std::size_t equal_pos = arg.find('=');
    bool is_named = (equal_pos != std::string::npos);
    if (is_named && equal_pos < 2) {
        std::cerr << "Invalid short argument format: " << arg << std::endl;
        return ValidationResult::kError;
    }
    if (!is_named && arg.size() > 2) {
        if (!ParseShortFlags(arg))
            return ValidationResult::kError;
        return ValidationResult::kHandled;
    } 
    if (is_named && equal_pos != 2) {
        std::cerr << "Invalid short argument format: " << arg << std::endl;
        return ValidationResult::kError;
    }
    return ValidationResult::kValid;
}

bool ArgParser::NextValueExists(ParseContext& ctx) const {
    if (!ctx.is_named && ctx.index + 1 >= ctx.args.size()) {
        std::cerr << "Expected value for argument: " << ctx.full_name << std::endl;
        return false;
    }
    return true;
}

bool ArgParser::ParseArgument(ParseContext& context) {
    switch (GetArgument(context.full_name)->GetType()) {
        case ArgumentType::kString: {
            return ParseStringArgument(context);
        }
        case ArgumentType::kInt: {
            return ParseIntArgument(context);
        }
        case ArgumentType::kFlag: {
            return ParseFlagArgument(context);
        }
    }
    return true;
}

bool ArgParser::ParseStringArgument(ParseContext& ctx) {
    if (!NextValueExists(ctx))
        return false;

    ArgPtr& argument = GetArgument(ctx.full_name);
    std::string value;
    if (ctx.is_named) {
        value = std::move(ctx.args[ctx.index].substr(ctx.equal_pos + 1));
    } else {
        value = ctx.args[++ctx.index];
    }
    dynamic_cast<StringArg&>(*argument).AddValue(std::move(value));
    return true;
}

bool ArgParser::ParseIntArgument(ParseContext& ctx) {
    ArgPtr& argument = GetArgument(ctx.full_name);
    if (!NextValueExists(ctx))
        return false;
    int64_t number;
    const char* value_str = ctx.is_named
                            ? ctx.args[ctx.index].c_str() + ctx.equal_pos + 1
                            : ctx.args[++ctx.index].c_str();
    if (!ConvertToNumber(value_str, number)) {
        std::cerr << "Invalid integer value for argument: " << ctx.full_name << std::endl;
        return false;
    }
    dynamic_cast<IntArg&>(*argument).AddValue(number);
    return true;
}

bool ArgParser::ParseFlagArgument(ParseContext& ctx) {
    if (ctx.is_named) {
        std::cerr << "Flag can not be named argument: " << ctx.full_name << std::endl;
        return false;
    }
    dynamic_cast<FlagArg&>(*GetArgument(ctx.full_name)).AddValue(true);
    return true;
}

bool ArgParser::IsNamedArgument(const std::string& arg) const {
    return arg.find('=') != std::string::npos;
}

bool ArgParser::ParseFullArgument(const std::vector<std::string>& args, size_t& i) {
    const std::string& arg = args[i];
    bool is_named = IsNamedArgument(arg);
    if (!is_named && arg.substr(2) == full_help_name_) {
        has_help_ = true;
        return true;
    }

    std::size_t equal_pos = arg.find('='); 
    std::string full_name = is_named 
                            ? arg.substr(2, equal_pos - 2)
                            : arg.substr(2);

    if (!argument_indices_.contains(full_name)) {
        std::cerr << "Unknown argument: " << full_name << std::endl;
        return false;
    }

    ParseContext context{args, full_name, i, equal_pos, is_named};
    return ParseArgument(context);
}

bool ArgParser::ParsePositionalArgument(const std::vector<std::string>& args, size_t& i) {
    if (positional_argument_name_.empty() && !SetPositionalArgument()) {
        std::cerr << "No positional argument defined." << std::endl;
        return false;
    }
    auto& argument = GetArgument(positional_argument_name_);
    switch(argument->GetType()) {
        case ArgumentType::kString: {
            return ParsePositionalStringArgument(argument, args[i]);
        }
        case ArgumentType::kInt: {
            return ParsePositionalIntArgument(argument, args[i]);
        }
        case ArgumentType::kFlag: {
            return ParsePositionalFlagArgument();
        }
    }
    return true;
}

bool ArgParser::ParsePositionalStringArgument(ArgPtr& argument, const std::string& str) {
    dynamic_cast<Argument<std::string>&>(*argument).AddValue(str);
    return true;
}

bool ArgParser::ParsePositionalIntArgument(ArgPtr& argument, const std::string& str) {
    int64_t number;
    if (!ConvertToNumber(str.c_str(), number)) {
        std::cerr << "Invalid integer value for argument: " 
                    << positional_argument_name_ << std::endl;
        return false;
    }
    dynamic_cast<Argument<int64_t>&>(*argument).AddValue(number);
    return true;
}

bool ArgParser::ParsePositionalFlagArgument() {
    std::cerr << "Flag is not supported for positional arguments." << std::endl;
    return false;
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
        dynamic_cast<FlagArg&>(*GetArgument(full_name)).AddValue(true);
    }
    return true;
}

bool ArgParser::ConvertToNumber(const char* str, int64_t& number) {
    auto [_, ec] = std::from_chars(str, str + strlen(str), number);
    return ec == std::errc{};
}

char ArgParser::GetShortName(std::string const& full_name) const {
    auto it = short_names_.find(full_name);
    if (it != short_names_.end()) {
        return it->second;
    }
    return '\0';
}

auto ArgParser::GetArgument(const std::string& full_name) 
        -> std::unique_ptr<BaseArgument>& {
    if (!argument_indices_.contains(full_name))
        throw std::invalid_argument("Unknown argument: " + full_name);
    return arguments_[argument_indices_[full_name]];
}

auto ArgParser::GetArgument(const std::string& full_name) const
        -> const std::unique_ptr<BaseArgument>& {
    if (!argument_indices_.contains(full_name))
        throw std::invalid_argument("Unknown argument: " + full_name);
    return arguments_[argument_indices_.at(full_name)];
}

void ArgParser::SetNames(char short_name, const std::string& full_name) {
    short_names_[full_name] = short_name;
    full_names_[short_name] = full_name;
}

void ArgParser::SetDescription(std::string& full_name, std::string& description) {
    descriptions_[full_name] = std::move(description);
    argument_indices_[std::move(full_name)] = arguments_.size();
}

void ArgParser::SetArgument(char short_name, std::string& full_name, std::string& description) {
    SetNames(short_name, full_name);
    SetDescription(full_name, description);
}

void ArgParser::CheckShortName(char short_name) const {
    if (!isalpha(short_name))
        throw std::invalid_argument("Short name must be a single alphabetic character");
}
}