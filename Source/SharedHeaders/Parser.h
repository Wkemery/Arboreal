// parser.h
//

#include <string>
#include <iostream>
#include <vector>
#include "ErrorCodes.h"
#include "CommandCodes.h"

typedef unsigned int uint;

class ParseError
{
public:
    /*!
     * @param where Where the parse error took place
     * 
     * @param what What the parse error consisted of
     */
    ParseError(const char* where, const char* what)
    {
        _where = where;
        _what = what;
    }

    /** @name Accessor Functions 
    */
    ///@{
    
    /*!
     * @return A std::string detailing where the parse error occured
     */
    std::string where(){return _where;}

    /*!
     * @return A std::string detailing what the parse error consisted of
     */
    std::string what() {return _what;}
    ///@}
private:
    std::string _where;
    std::string _what;

};



class Parser
{
public:

    /** @using name Constructors;
     */
    ///@{
    
    /*!
     * @param buffer A C-Style String representation of the string to be parsed
     * 
     * @param cwd    A C-Style String representation of the current working directory;
     *               (This value is typically provided by the Liaison process). 
     *               The directory string is used to parse commands which act within directories only 
     *               thus providing commands such as 'tag' a "path" to the file(s) which will be tagged 
     *               without the user having to explicitly enter those file's entire paths themselves.
     * 
     * @param max_name_size The maximum length that a file or tagname is allowed to have;
     *                      (This value is typically provided by the Liaison process)
     */
    Parser(char* buffer, char* cwd, int max_name_size);

    /*!
     * @param buffer A std::string representation of the string to be parsed
     * 
     * @param cwd   A String Literal representation of the current working directory;
     *              (This value is typically provided by the Liaison process). 
     *              The directory string is used to parse commands which act within directories only 
     *              thus, providing commands such as 'tag' a "path" to the file(s) which will be tagged 
     *              without the user having to explicitly enter those file's entire paths themselves.
     * 
     * @param max_name_size The maximum length that a file or tagname is allowed to have;
     *                      (This value is typically provided by the Liaison process)
     */
    Parser(std::string string, std::string cwd, int max_name_size);

    /*!
     * @param buffer A String Literal representation of the string to be parsed
     * 
     * @param cwd    A String Literal representation of the current working directory;
     *               (This value is typically provided by the Liaison process). 
     *               The directory string is used to parse commands which act within directories only 
     *               thus, providing commands such as 'tag' a "path" to the file(s) which will be tagged 
     *               without the user having to explicitly enter those file's entire paths themselves.
     * 
     * @param max_name_size The maximum length that a file or tagname is allowed to have;
     *                      (This value is typically provided by the Liaison process)
     */
    Parser(const char* string_lit, const char* cwd, int max_name_size);

    /*!
     * Default Constructor to be used in case initialization of values needs to be done elsewhere
     */
    Parser();
    ///@}
    
    /*!
     * Default Destructor; Does nothing
     */
    ~Parser();

    /** @name Public Mutators 
    */
    ///@{
    
    /*! \brief Changes the member _string of the parser class to whatever is passed.
     * 
     * The Parser class conducts all operations on its member _string rather than requiring
     * that a string value be passed to its parse() method.  This was done in order to make use 
     * of the class as streamlined as possible.
     * 
     * @param string A std::string representation of the string to be parsed
     * 
     * @param cwd    A std::string representation of the current working directory; Note 
     *               that this argument is optional and allows the user to both reset the string the Parser will
     *               work with as well as the directory string the Parser will use.  The directory string is used to parse
     *               commands which act within directories only thus providing commands such as 'tag' a "path" to the file(s)
     *               which will be tagged without the user having to explicitly enter those file's entire paths themselves.
     */
    void reset(std::string string, std::string cwd="");

    /*! \brief Changes the member _string of the parser class to whatever is passed.
     *
     * The Parser class conducts all operations on its member _string rather than requiring
     * that a string value be passed to its parse() method.  This was done in order to make use 
     * of the class as streamlined as possible.
     * 
     * @param string A C-Style String representation of the string to be parsed
     * 
     * @param cwd    A C-Style String representation of the current working directory; Note 
     *               that this argument is optional and allows the user to both reset the string the Parser will
     *               work with as well as the directory string the Parser will use.  The directory string is used to parse
     *               commands which act within directories only thus providing commands such as 'tag' a "path" to the file(s)
     *               which will be tagged without the user having to explicitly enter those file's entire paths themselves.
     *
     * @return Void
     */
    void reset(char* buffer, char* cwd=NULL);

    /*! \brief Changes the member _string of the parser class to whatever is passed.
     *
     * The Parser class conducts all operations on its member _string rather than requiring
     * that a string value be passed to its parse() method.  This was done in order to make use 
     * of the class as streamlined as possible.
     * 
     * @param string A String Literal representation of the string to be parsed
     * 
     * @param cwd    A String Literal representation of the current working directory; Note 
     *               that this argument is optional and allows the user to both reset the string the Parser will
     *               work with as well as the directory string the Parser will use.  The directory string is used to parse
     *               commands which act within directories only thus providing commands such as 'tag' a "path" to the file(s)
     *               which will be tagged without the user having to explicitly enter those file's entire paths themselves.
     *
     * @return Void
     */
    void reset(const char* string_lit, const char* cwd="");

    /*! \brief Sets the maximum allowed file and tagname size that the Parser will use.
     * 
     * If this size is exceeded an error is thrown and the Parser will stop its
     * current activities.  This value is dictated by the CLI and is generally provided
     * to the Parser by the Liaison Process.
     * 
     * @param size The maximum file/tag name length
     */
    void set_max_name_size(int size);

    /*! \brief Sets the Current Working Directory that the Parser will use.
     *
     * The directory string is used to parse commands which act within directories only 
     * thus providing commands such as 'tag' a "path" to the file(s) which will be tagged without the user 
     * having to explicitly enter those file's entire paths themselves.
     * This function does not have counterparts which tahe C-Style Strings or String Literals.  This is because,
     * in all situations, if the current working directory must be set using this method, it is highly likely that
     * the calling code has a std::string representation of the current working directory rather than a representation
     * in one of the other formats.  If such functionality (C-Style Strings and others) is desired, extensibility is
     * easy enough.  Regardless the Parser's _cwd member will always be a std::string.
     * 
     * @param cwd A std::string representation of the current working directory
     *
     * @return Void
     */
    void set_cwd(std::string cwd);
    ///@}
    
    /** @name Public Accessors
     */
    ///@{
    
    /*! \brief Parse a string based on a certain rule.
     *
     * The rule generally corresponds to how a CLI command should be decomposed.  
     * For example the CLI command for finding files takes a list of files, 
     * hower the CLI itself does not support batch commands, therefore, 
     * the Parser will decompose the command into its constituent parts (i.e. a single file).  
     * This particular behavior is access by passing '8' as the "type" of 
     * decomposition that needs to take place (Note that this corresponds to the command's ID).  
     * However the Parser can be extended to support any rule whatsoever,
     * so long as it is added to the Parser's parse() function switch statement.
     *
     * @param type The integer identification of the parse rule that will be executed
     *
     * @return A std::vector of std::string comprised of the result after the chosen parse rule
     * is executed.
     */
    std::vector<std::string> parse(int type);

    /*! \brief  Returns a vector representation of the current working directory.
     * 
     * That is, it will decompose '/string1/string2' into a vector containing 
     * [string1, string2].  This is useful when the calling code requires the current working
     * directory as a vector of strings rather than as a standard string representation.
     *
     * @return A std::vector of std::string comprised of the non-'/' parts of the 
     * Parser member value _cwd
     */
    std::vector<std::string> get_cwd_tags();

    /*! \brief Splits a string at each instance of a particualar char (the delimeter)
     * 
     * 
     * The delimeters are NOT included anywhere in the resulting vector.
     * This function is static and is mainly used outside the Parser in order
     * to split values that the parser returned.  This can happen because the complexity
     * of certain commands does not allow the parser to fully decompose the string and instead
     * it can only reorganize the command into a form which can be easily split later.  It is important
     * to note that this function does not differentiate between the number of delimeter charcters the string
     * contains.  That is, it will read the whole string and split it at any point where the delimeter is seen
     * whether it is seen in 1 or 100 places.
     *
     * @param string A std::string representation of whatever string needs to be split
     * @param delim A char value representing where the string should be split 
     */
    static std::vector<std::string> split_on_delim(std::string string, char delim);
    ///@}
    

private:

    /** @name Private Helper Functions 
    */
    ///@{
    
    /*!
     * Helper function to recursively decompose commands of the form 'command [s1,s2,..]'.
     * Is also instrumental in the parsing of 'find' commands in instances were the
     * find command includes set notation (e.g. 'find [s1,{s2,s3},...]').
     * 
     * @param string A std::string representation of the string which will be decomposed
     * 
     * @return A std::vector of std::string representing the decomposed string
     */
    std::vector<std::string> lunion(std::string string);

    /*!
     * Helper function to recursively decompose commands of the form 'command {s1,s2,..}'.
     * Is also instrumental in the parsing of 'find' commands in instances were the
     * find command includes list notation (e.g. 'find {s1,[s2,s3],...}').
     * 
     * @param string A std::string representation of the string which will be decomposed
     * 
     * @return A std::vector of std::string representing the decomposed string
     */
    std::vector<std::string> lintersect(std::string string);

    /*! \brief Helper function which performs matrix multiplication on two std::vector's of std::string's
     * Always multiplies the larger vector into the smaller vector unless the vectors are of equal size.
     * 
     * Used to create strings for 'find' commands which are of the form '{s1,[s2,s3],...}'.  The '{}' 
     * construction is an && construction, that is, it finds all files tagged with ALL of the strings
     * within the braces.  However, the '[]' construction is an || construction and finds all files tagged with ANY of the 
     * strings vithin the brackets.  This means that a construction like '{s1,[s2,s3],s4..}' must 
     * find files tagged with: {s1,s2,s4..}, {s1,s3,s4..} the best way to achieve these results
     * is to matrix multiply the vector returned by lunion() (which handles '[]' constructions) and
     * lintersect() (which handles '{}' constructions).  Note that this has cubic time comlexity 
     * and so should be used sparingly for very large vectors.
     * 
     * @param v1 One of the std::vector's of std::string's that will be multiplied
     * @param v2 The other of the std::vector's of std::string's that will be multiplied
     * 
     * @return A std::vector of std::string's representing the matrix multiplication of v1 & v2
     */
    std::vector<std::string> matrix_multiply(std::vector<std::string> v1,std::vector<std::string> v2);

    /*! \brief Removes unnescesarry information (such as the command flags),
     *         from a command string which is contains a "path"
     * 
     * For example, the command 'delete /tag1/tag2/file' would be reduced to '/tag1/tag2/file'
     *  
     * @param parsed A referance to the std::vector into which the reduced command will be added.
     * In general this should be the std::vector that the Parser will return upon successful completion of parse()
     * 
     * @return Void
     */
    void parse_path(std::vector<std::string>& parsed);

    /*! \brief Handles the parsing of 'rename tag' commands
     *
     * The 'rename tag' commands must decomposes two seperate lists and then join
     * each value at index = x in either list together.  That is, the value at index 1 in list 1 must
     * be joined with the value at index 1 in list 2.  parse_rname() performs this function and 
     * adds the result to the vector that the Parser will return upon the successful completion of parse(),
     * in the following format 'list1value-list2value'.  In an effort to reduce "Walls of Text" and
     * because of the uniqueness of this command, this particular algorithm was placed into its own function.
     * 
     * @param parsed A referance to the std::vector into which the reduced command will be added.
     *               In general this should be the std::vector that the Parser will return 
     *               upon successful completion of parse()
     * 
     * @return Void
     */
    void parse_rename(std::vector<std::string>& parsed);

    /*! \brief Handles the parsing of 'merge tag' commands
     *
     * The 'merge tags' commands have syntax that does not conorm well with the rest of the commands
     * Therefore because of their uniqueness and in an effort to reduce "Walls of Text" they were separated into
     * their own function.  parse_merge() will decompose the command 'merge [t1,t2,...] => tx' into multiple
     * single merge operations of the form 't1-tx', 't2-tx', etc. and add them to the vector that the Parser will 
     * return upon successful completion of parse()
     *  
     * @param parsed  A referance to the std::vector into which the reduced command will be added.
     *                In general this should be the std::vector that the Parser will return 
     *                upon successful completion of parse()
     */
    void parse_merge(std::vector<std::string>& parsed);

    /*! \brief Set index to the first instance of the requested char within a std::string
     *
     * Because fast forwarding to a specific index (for example in order to skip command flags
     * or other unimportant data (to the parser anyway)) is such a common need, this function was made
     * in order to reduce code duplication.  It use the member _string as the std::string it operates on
     * and will return the index within the std::string at which the chose delimeter occurs.  It DOES NOT
     * modify the underling string in any way.
     * 
     * @param index A reference to a variable that will be incremented as the function operates
     * 
     * @param delim A char value which will function as the delimeter at which the function will stop
     * 
     * @return Void
     */
    void jump_to_char(int& index, char delim);

    /*! \brief Generates a std::string from a given index up until a chosen delimiting charachter
     *         Using another std::string as its base
     *
     * Because the process of creating substrings from starting at an arbitrary index and ending
     * at a specific charachter is so common, this function was created in order to reduce code duplication.
     * This function will generate a string from a given index up until a chosen delimiting charachter. 
     * It also saves the resulting end index.  This function DOES NOT modify the underling std::string in any way
     * and like other functions above, uses the member _string as its underlying std::string.
     * 
     * @param index     A reference to a variable that will be incremented as the function operates.  This serves as
     *                  both the starting index and the will store the resulting index after the function completes.
     * 
     * @param string    A reference to a std::string that will be modified with a subset of the contents of 
     *                  the base string.
     *                  
     * @param delim     The delimiting charachter which will mark the stopping point of the function
     * 
     * @return Void
     */
    void substr_to_char(int& index, std::string& string, char delim);
    ///@}
    

    std::string _string;
    std::string _cwd;
    int _max_name_size;
};
