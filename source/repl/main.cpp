#include <cimm/parse.hpp>
#include <cimm/eval.hpp>
#include <cimm/str.hpp>
#include <cimm/default_environment.hpp>
#include <cimm/error.hpp>
#include <iostream>

void eval_line(cimm::environment& env, const std::string& line)
{
    try
    {
        auto expr = cimm::parse_expression(line);
        auto result = cimm::evaluate_expression(env, expr);
        std::cout << "> " << cimm::pr_str(result) << std::endl;
    }
    catch (cimm::parse_error const& e)
    {
        std::cout << "error: " << e.what() << std::endl;
    }
    catch (cimm::evaluation_error const& e)
    {
        std::cout << "error: " << e.what() << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cout << "internal error: " << e.what() << std::endl;
    }
}

int main()
{
    auto environment = cimm::create_default_environment();
    std::string line;

    while ((std::cout << "$ " << std::flush) && std::getline(std::cin, line))
        eval_line(environment, line);
}
