// parser.h
//

#include <string>
#include <iostream>
#include <vector>

typedef unsigned int uint;

class Parser
{
public:
    Parser(char* buffer, char* cwd);
    Parser(std::string string,std::string cwd);
    Parser(const char* string_lit, const char* cwd);
    Parser();
    ~Parser();
    void reset(std::string string, std::string cwd="");
    void reset(char* buffer, char* cwd=NULL);
    void reset(const char* string_lit, const char* cwd="");
    std::vector<std::string> parse(int type);
    std::vector<std::string> get_cwd_tags();
private:
    std::vector<std::string> lunion(std::string string);
    std::vector<std::string> lintersect(std::string string);
    std::vector<std::string> matrix_multiply(std::vector<std::string> v1,std::vector<std::string> v2);
    void parse_path(std::vector<std::string>& parsed);
    void parse_rename(std::vector<std::string>& parsed);
    void parse_merge(std::vector<std::string>& parsed);
    std::string _string;
    std::string _cwd;
};
