# Clap++

Low quality header-only C++ command line argument parser library inspired by
[clap](https://github.com/clap-rs/clap).

## Usage Example

```cpp
#include "clapp.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    clapp::app app("FooBar");

    auto result = app.version("1.0")
                      .description("Example Program")
                      .arg(app.new_arg("first_number").required(true).value_name("FIRST"))
                      .arg(app.new_arg("second_number").required(true).value_name("SECOND"))
                      .arg(app.new_arg("operator")
                               .short_flag('o')
                               .long_flag("operator")
                               .takes_value(true)
                               .default_value("+")
                               .description("Valid operators: +, -, *, /"))
                      .arg(app.new_arg("prefix").short_flag('p').long_flag("prefix").takes_value(true))
                      .arg(app.new_arg("suffix").short_flag('s').long_flag("suffix").takes_value(true))
                      .parse(argc, argv);

    if (!result) {
        std::cout << "error: " << result.error_message() << std::endl;
        return -1;
    }

    if (result.is_present("help")) {
        std::cout << app.help() << std::endl;
        return 0;
    }

    if (result.is_present("version")) {
        std::cout << app.version() << std::endl;
        return 0;
    }

    auto first_number = std::stof(result.value_of("first_number"));
    auto second_number = std::stof(result.value_of("second_number"));
    auto operation = result.value_of("operation");
    auto prefix = result.value_of("prefix");
    auto suffix = result.value_of("suffix");
    float operation_result = 0;

    if (operation == "+") {
        operation_result = first_number + second_number;
    } else if (operation == "-") {
        operation_result = first_number - second_number;
    } else if (operation == "*") {
        operation_result = first_number * second_number;
    } else if (operation == "/") {
        operation_result = first_number / second_number;
    } else {
        std::cout << "Invalid operation" << std::endl;
        return -1;
    }

    std::cout << prefix << operation_result << suffix << std::endl;

    return 0;
}
```

## TO-DO

- [ ] Tests
- [ ] Subcommands
