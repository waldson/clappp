#pragma once

#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace clapp {

struct app;
struct parse_result;

struct arg
{
    friend class app;

    arg(const std::string name) :
        _name(name),
        _short_flag('\0')
    {
    }

    const std::string name() const
    {
        return this->_name;
    }

    arg& name(const std::string& name)
    {
        this->_name = name;
        return *this;
    }

    char short_flag() const
    {
        return this->_short_flag;
    }

    arg& short_flag(char short_flag)
    {
        this->_short_flag = short_flag;
        return *this;
    }

    const std::string long_flag() const
    {
        return this->_long_flag;
    }

    arg& long_flag(const std::string& long_flag)
    {
        this->_long_flag = long_flag;
        return *this;
    }

    const std::string description() const
    {
        return this->_description;
    }

    arg& description(const std::string& description)
    {
        this->_description = description;
        return *this;
    }

    const std::string value_name() const
    {
        return this->_value_name.empty() ? this->_name : this->_value_name;
    }

    arg& value_name(const std::string& value_name)
    {
        this->_value_name = value_name;
        return *this;
    }

    const std::string default_value() const
    {
        return this->_default_value;
    }

    arg& default_value(const std::string& default_value)
    {
        this->_default_value = default_value;
        return *this;
    }

    bool positional() const
    {
        return _short_flag == '\0' && _long_flag.empty();
    }

    bool multiple() const
    {
        return this->_multiple;
    }

    arg& multiple(bool multiple)
    {
        this->_multiple = multiple;
        return *this;
    }

    bool required() const
    {
        return this->_required;
    }

    arg& required(bool required)
    {
        this->_required = required;
        return *this;
    }

    bool takes_value() const
    {
        return this->_takes_value;
    }

    bool flag() const
    {
        return !_takes_value && !positional();
    }

    bool argument() const
    {
        return !positional() && !flag();
    }

    bool is_option() const
    {
        return !_takes_value && !_multiple;
    }

    arg& takes_value(bool takes_value)
    {
        this->_takes_value = takes_value;
        return *this;
    }

    arg& requires(const std::string other_arg_name)
    {
        _requires_args.push_back(other_arg_name);
        return *this;
    }

    arg& clear_requires()
    {
        _requires_args.clear();
        return *this;
    }

    arg& requires_all(const std::vector<std::string> other_args_names)
    {
        for (auto& arg : other_args_names) {
            _requires_args.push_back(arg);
        }
        return *this;
    }

    arg& conflicts(const std::string other_arg_name)
    {
        _conflicts_args.push_back(other_arg_name);
        return *this;
    }

    arg& clear_conflicts()
    {
        _conflicts_args.clear();
        return *this;
    }

    arg& conflicts_all(const std::vector<std::string> other_args_names)
    {
        for (auto& arg : other_args_names) {
            _conflicts_args.push_back(arg);
        }
        return *this;
    }

  private:
    std::string _name;
    std::string _value_name;
    std::string _default_value;
    char _short_flag;
    std::string _long_flag;
    std::string _description;
    bool _multiple = false;
    bool _takes_value = false;
    bool _required = false;
    std::vector<std::string> _requires_args;
    std::vector<std::string> _conflicts_args;
    /* std::vector<clapp::app> subcommands; */
    size_t position;
};

struct result
{
    friend class clapp::parse_result;

    inline operator bool()
    {
        return _values.size() > 0;
    }

    result& add_ocurrence()
    {
        _values.push_back("");
        return *this;
    }

    result& add_ocurrence(const std::string& value)
    {
        _values.push_back(value);
        return *this;
    }

  private:
    std::vector<std::string> _values;
};

struct parse_result
{
    friend class clapp::app;

    inline bool is_present(const std::string arg_name)
    {
        return _values.find(arg_name) != _values.end() && _values[arg_name]._values.size() > 0;
    }

    /* inline bool value_of(const std::string arg_name); */
    inline size_t ocurrences_of(const std::string arg_name)
    {
        if (!is_present(arg_name)) {
            return 0;
        }

        return _values[arg_name]._values.size();
    }

    inline operator bool()
    {
        return !_error;
    }

    std::string error_message()
    {
        return _error_message;
    }

    std::string value_of(const std::string& arg_name, const std::string& default_value)
    {
        if (!is_present(arg_name)) {
            return default_value;
        }

        return _values[arg_name]._values[0];
    }

    std::string value_of(const std::string& arg_name)
    {
        return value_of(arg_name, "");
    }

    std::vector<std::string> values_of(const std::string& arg_name)
    {
        if (!is_present(arg_name)) {
            return {};
        }
        return _values[arg_name]._values;
    }

  private:
    std::unordered_map<std::string, clapp::result> _values;
    bool _error = false;
    std::string _error_message;
};

struct app
{
    app(const std::string name) :
        _name(name)
    {
        arg(
            new_arg("help")
                .short_flag('h')
                .long_flag("help")
                .description("Prints this help"));
        arg(
            new_arg("version")
                .short_flag('V')
                .long_flag("version")
                .description("Prints version"));
    }

    app& name(const std::string& name)
    {
        this->_name = name;
        return *this;
    }

    const std::string name() const
    {
        return this->_name;
    }

    const std::string bin_name() const
    {
        return this->_bin_name;
    }

    app& bin_name(const std::string& bin_name)
    {
        this->_bin_name = bin_name;
        return *this;
    }

    const std::string description() const
    {
        return this->_description;
    }

    app& description(const std::string& description)
    {
        this->_description = description;
        return *this;
    }

    const std::string version() const
    {
        return this->_version;
    }

    app& version(const std::string& version)
    {
        this->_version = version;
        return *this;
    }

    const std::string license() const
    {
        return this->_license;
    }

    app& license(const std::string& license)
    {
        this->_license = license;
        return *this;
    }

    const std::string author() const
    {
        return this->_author;
    }

    app& author(const std::string& author)
    {
        this->_author = author;
        return *this;
    }

    app& arg(const arg& arg)
    {
        _args.push_back(arg);
        return *this;
    }

    clapp::arg new_arg(const std::string arg_name) const
    {
        return clapp::arg{arg_name};
    }

    clapp::parse_result parse(int argc, char* argv[])
    {
        if (_bin_name.empty()) {
            bin_name(argv[0]);
        }

        size_t current_position = 0;
        clapp::arg* pending_arg = nullptr;
        clapp::parse_result result;
        bool only_values = false;

        for (int i = 1; i < argc; ++i) {
            std::string arg_string = argv[i];

            if (!only_values && is_long_flag(arg_string)) {
                if (pending_arg != nullptr) {
                    if (!pending_arg->flag()) {
                        result._error = true;
                        result._error_message = concat("Missing value for option: ", pending_arg->name());
                        return result;
                    }
                }
                pending_arg = nullptr;

                std::string flag{arg_string, 2};
                clapp::arg* arg = find_by_long_flag(flag);

                if (arg == nullptr) {
                    result._error = true;
                    result._error_message = concat("Invalid option: ", flag);
                    return result;
                }
                pending_arg = parse_arg(result, *arg);
            } else if (!only_values && is_short_flag(arg_string)) {
                if (pending_arg != nullptr) {
                    if (!pending_arg->flag()) {
                        result._error = true;
                        result._error_message = concat("Missing value for option: ", pending_arg->name());
                        return result;
                    }
                }
                pending_arg = nullptr;
                std::string flag{arg_string, 1};
                for (auto& c : flag) {
                    clapp::arg* arg = find_by_short_flag(c);

                    if (arg == nullptr) {
                        result._error = true;
                        result._error_message = concat("Invalid option: ", c);
                        return result;
                    }
                    pending_arg = parse_arg(result, *arg);
                }
            } else { //value
                if (!only_values && arg_string == "--") {
                    only_values = true;
                    continue;
                }

                if (pending_arg != nullptr) {
                    if (pending_arg->takes_value()) {
                        result._values[pending_arg->name()].add_ocurrence(arg_string);
                    }
                    pending_arg = nullptr;
                    continue;
                }

                clapp::arg* position_arg = find_by_position(current_position);
                if (position_arg == nullptr) {
                    result._error = true;
                    result._error_message = concat("Unexpected argument: ", arg_string);
                    return result;
                }
                result._values[position_arg->name()].add_ocurrence(arg_string);
                current_position++;
            }

            if (!result) {
                return result;
            }
        }

        if (result.is_present("help")) {
            return result;
        } else if (result.is_present("version")) {
            return result;
        } else {
            validate_and_put_default_values(result);
        }

        return result;
    }

    inline std::string help()
    {
        using argument_pair = std::vector<std::pair<std::string, std::string>>;

        std::vector<clapp::arg*> options;
        std::vector<clapp::arg*> positional;
        std::vector<clapp::arg*> flags;

        for (auto& arg : _args) {
            if (arg.flag()) {
                flags.push_back(&arg);
            } else if (arg.positional()) {
                positional.push_back(&arg);
            } else {
                options.push_back(&arg);
            }
        }

        std::stringstream stream;
        stream << _name;
        if (!_version.empty()) {
            stream << " " << _version;
        }

        stream << "\n";

        if (!_description.empty()) {
            stream << _description << "\n";
        }
        stream << "\n";

        stream << "USAGE:"
               << "\n";

        stream << bin_name();

        if (options.size() > 0 || flags.size() > 0) {
            stream << " [OPTIONS]";
        }

        if (positional.size() > 0) {
            for (auto arg : positional) {
                stream << " <" << arg->value_name() << ">";
            }

            stream << "\n";
            stream << "\nARGS:";

            argument_pair arg_pairs;
            size_t max_size = -std::numeric_limits<size_t>::infinity();

            for (auto arg : positional) {
                std::stringstream arg_stream;
                arg_stream << concat("<", arg->value_name(), ">");

                auto display = arg_stream.str();
                if (display.size() > max_size) {
                    max_size = display.size();
                }

                arg_pairs.push_back({display, arg->description()});
            }

            append_items_aligned(stream, arg_pairs, max_size);
        }

        if (flags.size() > 0) {
            stream << "\n";
            stream << "\nFLAGS:";

            argument_pair flag_pairs;
            size_t max_size = -std::numeric_limits<size_t>::infinity();

            for (auto arg : flags) {
                std::stringstream flag_stream;

                if (arg->short_flag() != '\0') {
                    flag_stream << concat("-", arg->short_flag());
                    if (!arg->long_flag().empty()) {
                        flag_stream << concat(", --", arg->long_flag());
                    }
                    if (arg->multiple()) {
                        flag_stream << concat(" <", arg->short_flag(), ">...");
                    }
                } else {
                    flag_stream << concat("--", arg->long_flag());
                    if (arg->multiple()) {
                        flag_stream << "...";
                    }
                }

                auto display = flag_stream.str();
                if (display.size() > max_size) {
                    max_size = display.size();
                }

                flag_pairs.push_back({display, arg->description()});
            }

            append_items_aligned(stream, flag_pairs, max_size);
        }

        if (options.size() > 0) {
            stream << "\n";
            stream << "\nOPTIONS:";

            argument_pair option_pairs;
            size_t max_size = -std::numeric_limits<size_t>::infinity();

            for (auto arg : options) {
                std::stringstream option_stream;

                if (arg->short_flag() != '\0') {
                    option_stream << concat("-", arg->short_flag());
                    if (!arg->long_flag().empty()) {
                        option_stream << concat(", --", arg->long_flag());
                    }
                    if (arg->multiple()) {
                        option_stream << concat(" <", arg->short_flag(), ">...");
                    }
                } else {
                    option_stream << concat("--", arg->long_flag());
                    if (arg->multiple()) {
                        option_stream << "...";
                    }
                }

                option_stream << concat(" <", arg->value_name(), ">");

                auto display = option_stream.str();
                if (display.size() > max_size) {
                    max_size = display.size();
                }

                option_pairs.push_back({display, arg->description()});
            }
            append_items_aligned(stream, option_pairs, max_size);
        }

        return stream.str();
    }

  private:
    std::string _name;
    std::string _description;
    std::string _version;
    std::string _author;
    std::string _bin_name;
    std::string _license;
    std::vector<clapp::arg> _args;

    inline void append_items_aligned(std::stringstream& stream, const std::vector<std::pair<std::string, std::string>>& items, size_t max_size)
    {
        for (auto& item : items) {
            stream << "\n";
            auto size = item.first.size();
            stream << "    " << item.first;
            if (!item.second.empty()) {

                for (size_t i = size; i <= max_size + 4; ++i) {
                    stream << " ";
                }
                stream << item.second;
            }
        }
    }

    inline void validate_and_put_default_values(clapp::parse_result& result)
    {
        for (auto& arg : _args) {
            if (!result.is_present(arg.name())) {
                auto default_value = arg.default_value();
                if (!default_value.empty()) {
                    result._values[arg.name()].add_ocurrence(default_value);
                }
            }

            if (arg.required() && !result.is_present(arg.name())) {
                result._error = true;
                result._error_message = concat("Missing required argument: ", arg.value_name());
                return;
            }

            if (arg.takes_value() && !result.is_present(arg.name())) {
                result._error = true;
                result._error_message = concat("Missing value for option: ", arg.value_name());
                return;
            }
        }

        for (auto& arg : _args) {
            for (auto& other_arg : arg._conflicts_args) {
                if (result.is_present(other_arg)) {
                    result._error = true;
                    result._error_message = concat(arg.name(), " can't be used with ", other_arg);
                    return;
                }
            }

            for (auto& other_arg : arg._requires_args) {
                if (!result.is_present(other_arg)) {
                    result._error = true;
                    result._error_message = concat(arg.name(), " should be used with ", other_arg);
                    return;
                }
            }
        }
    }

    inline clapp::arg* parse_arg(clapp::parse_result& result, clapp::arg& arg)
    {
        if (result.is_present(arg.name())) {
            if (!arg.multiple()) {
                result._error = true;
                result._error_message = concat("Multiple definition: ", arg.name());
                return nullptr;
            }
        }
        if (arg.flag()) {
            result._values[arg.name()].add_ocurrence();
            return nullptr;
        }

        return &arg;
    }

    inline bool is_long_flag(const std::string& option)
    {
        return option.size() > 2 && option[0] == '-' && option[1] == '-';
    }

    inline bool is_value(const std::string& option)
    {
        return !is_long_flag(option) && !is_short_flag(option);
    }

    inline bool is_short_flag(const std::string& option)
    {
        return option.size() >= 2 && option[0] == '-' && option[1] != '-';
    }

    inline std::string do_concat(std::stringstream& stream)
    {
        return stream.str();
    }

    template <typename T, typename... Args>
    inline std::string do_concat(std::stringstream& stream, T first, Args... args)
    {
        stream << first;
        return do_concat(stream, args...);
    }

    template <typename T, typename... Args>
    inline std::string concat(T first, Args... args)
    {
        std::stringstream stream;
        return do_concat(stream, first, args...);
    }

    clapp::arg* find_by_long_flag(const std::string long_flag)
    {
        for (auto& arg : _args) {
            if (arg.long_flag() == long_flag) {
                return &arg;
            }
        }

        return nullptr;
    }

    clapp::arg* find_by_short_flag(char short_flag)
    {
        for (auto& arg : _args) {
            if (arg.short_flag() == short_flag) {
                return &arg;
            }
        }

        return nullptr;
    }

    clapp::arg* find_by_position(size_t position)
    {
        size_t current_position = -1;

        for (auto& arg : _args) {
            if (arg.positional()) {
                current_position++;
            }

            if (current_position == position) {
                return &arg;
            }
        }

        return nullptr;
    }
};
}; // namespace clapp
