#include <cimm/parse.hpp>
#include <cimm/eval.hpp>
#include <cimm/default_environment.hpp>
#include <cimm/error.hpp>
#include <iostream>
#include <fstream>

void run(std::istream& is)
{
    std::string source{std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{}};
    auto exprs = cimm::parse_expressions(source);

    auto environment = cimm::create_default_environment();
    for (auto expr : exprs)
        cimm::evaluate_expression(environment, expr);
}

int main(int argc, const char *const *argv)
{
    try
    {
        if (argc == 1)
            run(std::cin);
        else
        {
            std::ifstream f(argv[1]);
            run(f);
        }
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
    catch (...)
    {
        std::cout << "internal error: unknown" << std::endl;
    }
}
