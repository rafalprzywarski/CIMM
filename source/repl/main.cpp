#include <cimm/parse.hpp>
#include <cimm/eval.hpp>
#include <iostream>

void eval_line(const std::string& line)
{
    try
    {
        auto expr = cimm::parse_expression(line);
        auto result = cimm::evaluate_expression(expr);
        std::cout << "> " << result << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cout << "error: " << e.what() << std::endl;
    }
}

int main()
{
    std::string line;

    while (1)
    {
        std::cout << "$ " << std::flush;
        if (!std::getline(std::cin, line))
            break;
        eval_line(line);
    }
}
