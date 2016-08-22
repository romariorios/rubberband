#ifndef SOURCEFILE_HPP
#define SOURCEFILE_HPP

#include <error.hpp>
#include <object.hpp>
#include <parse.hpp>

namespace rbb
{

namespace modloader
{

class sourcefile final
{
public:
    sourcefile(base_master *master);

    object load_module(const std::string &modname) const;
    object program_from_file(const std::string &filename) const;

    void add_path_list(const std::vector<std::string> &paths);
    inline void add_path(const std::string &path) { add_path_list({path}); }

private:
    base_master &_master;
    std::vector<std::string> _module_paths;
};

class could_not_open_file : public std::exception
{
public:
    could_not_open_file(const std::string &f) : _f{f} {}
    inline const char *what() const noexcept
    {
        return std::string{"Could not open file \"" + _f + "\""}.c_str();
    }

private:
    std::string _f;
};

class sourcefile_syntax_error : public rbb::syntax_error
{
public:
    sourcefile_syntax_error(const rbb::syntax_error &other, const std::string &filename) :
        syntax_error{other},
        _f{filename}
    {}

    inline const char *what() const noexcept
    {
        return (std::string{syntax_error::what()} + " of file " + _f).c_str();
    }

private:
    std::string _f;
};

}

}

#endif
