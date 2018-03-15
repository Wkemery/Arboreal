// parser.h
//

#include <string>
#include <iostream>
#include <vector>

typedef unsigned int uint;

class ParseError
{
public:
    ParseError(const char* where, const char* what)
    {
        _where = where;
        _what = what;
    }
    std::string where(){return _where;}
    std::string what() {return _what;}
private:
    std::string _where;
    std::string _what;

};

class Parser
{
public:
    Parser(char* buffer, char* cwd, int max_name_size);
    Parser(std::string string, std::string cwd, int max_name_size);
    Parser(const char* string_lit, const char* cwd, int max_name_size);
    Parser();
    ~Parser();
    void reset(std::string string, std::string cwd="");
    void reset(char* buffer, char* cwd=NULL);
    void reset(const char* string_lit, const char* cwd="");
    void set_max_name_size(int size);
    void set_cwd(std::string cwd);
    std::vector<std::string> parse(int type);
    std::vector<std::string> get_cwd_tags();
    static std::vector<std::string> split_on_delim(std::string s, char delim);
private:
    std::vector<std::string> lunion(std::string string);
    std::vector<std::string> lintersect(std::string string);
    std::vector<std::string> matrix_multiply(std::vector<std::string> v1,std::vector<std::string> v2);
    void parse_path(std::vector<std::string>& parsed);
    void parse_rename(std::vector<std::string>& parsed);
    void parse_merge(std::vector<std::string>& parsed);
    std::string _string;
    std::string _cwd;
    int _max_name_size;
};
