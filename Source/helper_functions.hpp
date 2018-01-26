////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  helper_functions.h
//  Helper Functions for CLI
//  Primary Author: Adrian Barberis
//  For "Arboreal" Senior Design Project
//  
//  Fri. | Jan. 26th | 2018 | 11:27 PM
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HELPER_FUNC
#define HELPER_FUNC
#define INCLUSIVE 0
#define EXCLUSIVE 1
#define NEW_AND_TAG 2
#define NEW_AND_TAG_EXC 3
#define MERGE_1 4
#define MERGE_2 5
#define TAG_1 6
#define TAG_2 7
#define TAG_3 8




//[================================================================================================]
/*  Used by CLI Process:
 *
 *  Print a list of legal commands
 *  Eventually I would like to change this to a more 'man pages' style output
 *  And make the 'help' command display only "Commonly Used" commands
 *  A lot of these commands have some quirks which are important to let the user know about
 *  But printing them all in a single print is not a very good idea (ergo the man pages)
 *  
 */
void print_help()
{
    cout << "+-----------------------------------------------------------------------------------+\n";
    cout << "|                                    Available Commands                             |\n";
    cout << "+-----------------------------------------------------------------------------------+\n";
    cout << "|                                                                                   |\n";
    cout << "|  1)  { 'help' | 'h' }--> Display this printout                                    |\n";
    cout << "|                                                                                   |\n";
    cout << "|  2)  { 'quit' | 'q' }--> Quit this interface                                      |\n";
    cout << "|                                                                                   |\n";
    cout << "|  3)  { 'history' }--> Display the last 10 commands inputted                       |\n";
    cout << "|                                                                                   |\n";
    cout << "|  4)  { 'find' '-t' '[tagName,...]' }--> Find file(s) by tag(s)                    |\n";
    cout << "|                                                                                   |\n";
    cout << "|  5)  { 'find' '-t' '{tagName,...}' }--> Find file with all tags                   |\n";
    cout << "|                                                                                   |\n";
    cout << "|  6)  { 'find' '-f' '[filename(.ext),...]' }--> Find files by name                 |\n";
    cout << "|                                                                                   |\n";
    cout << "|  7)  { 'new' '-t' '[tagName,...]' }--> Create tags                                |\n";
    cout << "|                                                                                   |\n";
    cout << "|  8) { 'new' '-f' '[filename.ext,...]' }--> Create files                           |\n";
    cout << "|                                                                                   |\n";
    cout << "|  9) { 'new' '-f' '[filename.ext,...]' '-t' '[tagName,...] }--> Create&Tag files   |\n";            
    cout << "|                                                                                   |\n";
    cout << "|  10) { 'new' '-f' '[filename.ext,...]' '-t' '{tagName,...}' }--> Create&Tag files |\n";
    cout << "|                                                                                   |\n";
    cout << "|  11) { 'delete' '-t' '[tagName,...]' }--> Delete tags                             |\n";
    cout << "|                                                                                   |\n";
    cout << "|  12) { 'delete' '-f' '[filename.ext,...]' }--> Delete files                       |\n";
    cout << "|                                                                                   |\n";
    cout << "|  13) { 'fdelete' '-t' '[tagName,...]' }--> Force delete tags                      |\n";
    cout << "|                                                                                   |\n";
    cout << "|  14) { 'open' '[filename.ext,...]' }--> Open files  PATH                              |\n";
    cout << "|                                                                                   |\n";
    cout << "|  15) { 'close' '[filename.ext,...]' }--> Close files                              |\n";
    cout << "|                                                                                   |\n";
    cout << "|  16) { 'rename' '-t' '[tagName,...]' '-n' '[newName,...]' }--> Rename Tags        |\n";
    cout << "|                                                                                   |\n";
    cout << "|  17) { 'rename' '-f' '[filename.ext,...]' '-n' '[newName,...]' }--> Rename files  |\n";
    cout << "|                                                                                   |\n";
    cout << "|  18) { 'get' '[filename.ext,...]' }--> Get attributes of files                    |\n";
    cout << "|                                                                                   |\n";
    cout << "|  19) { 'merge' 'tagName1' '->' 'tagName2' }--> Merge two existing tags            |\n";
    cout << "|                                                                                   |\n";
    cout << "|  20) { 'merge' '[tagName,...]' '->' 'tagName' }--> Merge multiple tags into one   |\n";
    cout << "|                                                                                   |\n";
    cout << "|  21) { 'tag' 'filename.ext' '->' 'tagName' }--> Tag a file with a tag             |\n";
    cout << "|                                                                                   |\n";
    cout << "|  22) { 'tag' 'filename.ext' '->' '[tagName,...]' }--> Add tags to file            |\n";
    cout << "|                                                                                   |\n";
    cout << "|  23) { 'tag' '[filename.ext,...]' '->' 'tagName' }--> Tag multiple files          |\n";
    cout << "|                                                                                   |\n";
    cout << "+-----------------------------------------------------------------------------------+\n";
    cout << "Arboreal >> ";
} 
//[================================================================================================]









//[================================================================================================]
// Used by CLI Process:
// 
// Print a Welcome Header
// You may ask,  "Is this actually necessarry?"
// I respond, "Yes, Yes it is."
// 
void print_header()
{
    cout << "\n[]==============================================================================[]\n";
    cout << "||                              Welcome To Arboreal                             ||\n";
    cout << "||------------------------------------------------------------------------------||\n";
    cout << "||              Enter 'help' or 'h' to see a list of available commands         ||\n";
    cout << "[]==============================================================================[]\n";
    cout << "\n\n";
    cout << "Arboreal >> ";
}
//[================================================================================================]









//[================================================================================================]
// Used by CLI::build()
// 
// Prints the contents of a buffer (Made to order for 'Command Buffers' but can be edited if need be)
// NOTE: The first four characters (or rather whatever happens to be the size of an integer)
//       are skipped since they need to be read all together to mean anything (avoids garbage output)
//       
void print_buffer(char* buff, int size)
{
    cout << "Buffer Contents: ";
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        cout << buff[i];
    }
    cout << endl << endl;
}
//[================================================================================================]





void print_buffer_to_out(char* buff, int size, ostream* out)
{
    for(unsigned int i = 0; i < size; i++)
    {
        *out << buff[i]; 
    }
    *out << endl;
}







//[================================================================================================]
/* Used by CLI::build()
 *
 * Checks the data within a buffer (Made to order for 'Command Buffers' but can be edited if need be)
 * This is a security feature (hopefully)
 * The only data allowed in a command buffer is aplhanumeric including "_" and the '\0' character
 * All other characters are considered errors and will trigger a "fix_buffer" call
 * This prevents garbage in the 'Command Buffers' and hopefully limits side channel attacks
 *
 * NOTE: As in the case of print_buffer() the first X characters are skipped
 *       Where X is the size of an integer.  This is especially important here
 *       otherwise the 'Command Id' data will be flagged as bad and will be overwritten
 *       with null characters ('\0')
 * 
 */
bool check_buffer(char* buff, int size)
{
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        string temp;
        temp += buff[i];
        if(!std::regex_match(temp,good_buffer) && buff[i] != '\0')
        {
            cout << "\n\nBad Buffer!\n";
            return false;
        }
    }
    return true;
}
//[================================================================================================]








//[================================================================================================]
/* Used by CLI::build()
 *
 * This is more of a debug function.
 * It can be used to test the "write_to_cmnd()" operations in order to make sure that they
 * are writing data in chunks of size = to "max_string_size" (even for data that is only 2 char long)
 *
 * NOTE: As in the case of print_buffer() the first X characters are skipped
 *       Where X is the size of an integer
 *       
 */
void check_buffer_partitioning(char* buffer,int size)
{
    bool is_null = false;
    int alpha_count = 0;
    int null_count = 0;
    int p_count = 1;
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        //cout << "Buffer [" << i << "]: " << buffer[i] << endl;
        if(buffer[i] == '\0' && !is_null){is_null = true;}
        else if(!is_null){alpha_count += 1;}
        else if(is_null)
        {
            null_count += 1;
            if(null_count > 64)
            {
                cout << "Partition " << p_count << " Consists Of:\n";
                cout << "   " << alpha_count << " non-null characters\n";
                cout << "Remaining Space is Free\n\n";
                return;
            }
            if(buffer[i] != '\0')
            {
                cout << "Partition " << p_count << " Consists Of:\n";
                cout << "   " << alpha_count << " non-null characters\n";
                cout << "   " << null_count << " null characters\n";
                cout << "   With a total character count of: " << alpha_count + null_count << endl;
                cout << endl;
                is_null = false;
                alpha_count = 1;
                null_count = 0;
                p_count += 1;
            }
        }
    }

}
//[================================================================================================]








//[================================================================================================]
// Used by CLI::build()
// 
// Fixes a buffer flagged as bad by check_buffer()
// It does this by overwriting all offending data with the '\0' character
// 
// NOTE: As in the case of print_buffer() the first X characters are ignored
//       Where X is the size of an integer
//       
void fix_buffer(char* buff,int size)
{
    int  non_null = 0;
    int fixed_count = 0;
    for(unsigned int i = sizeof(int); i < size; i++)
    {
        string temp;
        temp += buff[i];
        if(buff[i] != '\0')
        {
            non_null += 1;
        }
        if(!std::regex_match(temp,good_buffer) && buff[i] != '\0')
        {
            buff[i] = '\0';
            fixed_count += 1;
        }
    }
    cout << "\n\nBuffer Fixed!\n";
    cout << "+-------------------------------------------+\n";
    cout << "Non-Null Char Count: " << non_null << endl;
    cout << "Fixed " << fixed_count << " Character(s) In Buffer\n";
    cout << ".............................................\n";
}
//[================================================================================================]









//[================================================================================================]
// Used by CLI::build()
// 
// Convert the first X characters in a 'Command Buffer' to an integer value
// X is the size of an integer
int get_cmnd_id(char* cmnd)
{
    char temp[sizeof(int)];
    for(unsigned int i = 0; i < sizeof(int); i++)
    {
        temp[i] = cmnd[i];
    }

    int* id = (int*)temp;
    return *id;
}
//[================================================================================================]









//[================================================================================================]
// Used by CLI::build()
// 
// Main logic used to write Liason redable commands to the 'Command Buffer'
// A lot of the commands are built with similar syntax (this is on purpose)
// Thus it made sense to avoid duplication and use a helper function
// 
void write_to_cmnd(char* cmnd, string input, int offset, int version, int max_string_size)
{
    bool ignore = true;
    int temp_index = 0;
    char temp[max_string_size];

    // Zero out the temp buffer
    memset(temp,'\0',max_string_size);



    if(version == INCLUSIVE) // Command uses the '[]' rather than '{}'
    {
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '[')
                {
                    // begin list (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == ']')
            {
                // Write last element in list to cmnd buffer
                // write to cmnd buffer and reset temp buffer, increase offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // write item to temp pipe until you hit a comma
                // (which signals that you are moving on to another item)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma, write the temp buffer to the cmnd buffer
                // reset temp buffer, update offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == EXCLUSIVE) // Uses '{}' rather than '[]'
    {
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '{')
                {
                    // begin set (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
               else{continue;}
            }
            else if(input[i] == '}')
            {
                // Write last element in set to cmnd buffer
                // write to cmnd buffer and reset temp buffer, increase offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // write item to temp pipe until you hit a comma
                // (which signals that you are moving on to another item)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma, write the temp buffer to the cmnd buffer
                // reset temp buffer, update offset
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == NEW_AND_TAG)
    {
        // For the 'new' commands that ALSO tag we must deal with two seperate lists/sets 
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(input[i] == '[')
                {
                    // begin lists (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == '-')
            {
                // Write the flag to the buffer to be used as a delimiter later on
                // then reset temp and update offset
                ignore = true; // In order to ignore spaces after flag
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1];
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;

                // increment the index by one (so we don't read in the 't' of the flag)
                i += 1;
            }
            else if(input[i] == ']')
            {
                // Write out last element of list
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // Write list element to temp (to later write out to cmnd buffer)
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // You've hit a comma and need to write the element
                // you just read into temp buffer to the cmnd buffer
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == NEW_AND_TAG_EXC)
    {
        // Same as the above except the multiple tags listed make part of a set
        // rather than a list ('{}' rather than '[]')
        // 
        
        // Boolean variables to control when to execute which particular logic
        bool bracket = true;
        bool brace = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // skip the command name, flag, and first bracket
            {
                if(bracket && input[i] == '[')
                {
                    // begin list name (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else if(brace && input[i] == '{')
                {
                    // begin set (you don't want to ignore any of the data coming next)
                    ignore  = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == '-')
            {
                // Save flag (to cmnd buffer) for later use as delimiter
                // 
                ignore = true; // ignore spaces after flag
                bracket = false; // done with brackets
                brace = true; // need to do braces next
                              // 
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1];
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;

                // increment the index by one (so we don't read in the 't' of the flag)
                i += 1;
            }
            else if(bracket && input[i] == ']')
            {
                // Write final element in list
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(brace && input[i] == '}')
            {
                // Write final element in set
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] != ',')
            {
                // save current element info
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else
            {
                // Write current element info to cmnd buffer
                // before moving on to next element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
        }
    }
//[================================================================================================]

    else if(version == MERGE_1)
    {
        // for single tag into single tag:  merge(tag1,tag2)
        for(unsigned int i = 0; i < input.length(); i++)
        {
            // ignore the begin part i.e. the "merge" string of ther command
            if(i < 6){continue;} 
            else
            {
                if(input[i] == '-')
                {
                    // Save previous entry (before the flag was hit)
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // Write flag
                    temp[temp_index] = input[i];
                    temp[temp_index+1] = input[i+1];
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // increment the index by one (so we don't read in the '>' of the flag)
                    i += 1;
                    continue;
                }
                else if(input[i] != ' ')
                {
                    // Save current entry (and later write it to cmnd buffer)
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
            }
        }

        // Write last entry to buffer
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
    }
//[================================================================================================]

    else if(version == MERGE_2)
    {
        // for merging many tags into a single tag: merge([tag1,tag2,tag3],tag4)
        bool ignore = true;
        bool single = false; // tag to merge into is not a list need to handle differently

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the comand name and first flag
            {
                if(input[i] == '[')
                {
                    // begin list (you don't want to ignore any of the data coming next)
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else if(input[i] == ',')
            {
                // Write out current list element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] == ']')
            {
                // Write out last list element
                temp_index = 0;
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                offset += max_string_size;
            }
            else if(input[i] == '-')
            {
                // Write flag for use as delimiter later
                temp[temp_index] = input[i];
                temp[temp_index+1] = input[i+1];
                memcpy(cmnd + offset,temp,max_string_size);
                memset(temp,'\0',max_string_size);
                temp_index = 0;
                offset += max_string_size;

                // increment the index by one (so we don't read in the '>' of the flag)
                i += 1;
                single  = true;
                continue;
            }
            else if(input[i] != ',' && input[i] != ' ')
            {
                // Save current element
                temp[temp_index] = input[i];
                temp_index += 1;
            }
            else if(single)
            {
                if(input[i] == ' '){continue;} // skip space preceding the single tag
                else
                {
                    // Save the single tag
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
            }
        }

        // Write out the single tag
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
    }
//[================================================================================================]

    else if(version == TAG_1)
    {
        // tagging a single file with a single tag
        bool ignore = true;
        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the comand name and first flag
            {
                if(input[i] == ' ')
                {
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else
            {
                if(input[i] != ' ')
                {
                    // Save current element
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
                else
                {
                    // Write out first element (filename)
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
            }
        }

        // Write out last element (tag)
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
        offset += max_string_size;
    }
//[================================================================================================]

    else if(version == TAG_2)
    {
        // Tag a file with multiple tags 
        bool ignore = true;
        bool list = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // ignore the command name and first flag
            {
                if(input[i] == ' ')
                {
                    ignore = false;
                    continue;
                }
                else{continue;}
            }
            else
            {
                if(input[i] == ' ')
                {
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
                else
                {
                    if(input[i] == '-')
                    {
                        // Save flag for use as delimiter
                        temp[temp_index] = input[i];
                        temp[temp_index+1] = input[i+1];
                        memcpy(cmnd + offset,temp,max_string_size);
                        memset(temp,'\0',max_string_size);
                        temp_index = 0;
                        offset += max_string_size;

                        // skip the rest of the flag and the space after it
                        i += 2;
                    }
                    else
                    {
                        if(input[i] == '[') // need to work with a list now
                        {
                            list = true;
                            continue;
                        }
                        else if(list)
                        {
                            if(input[i] == ',')
                            {
                                // Write out current element
                                memcpy(cmnd + offset,temp,max_string_size);
                                memset(temp,'\0',max_string_size);
                                temp_index = 0;
                                offset += max_string_size;
                            }
                            else if(input[i] == ']')
                            {
                                // Write out last element
                                memcpy(cmnd + offset,temp,max_string_size);
                                memset(temp,'\0',max_string_size);
                                temp_index = 0;
                                offset += max_string_size;
                            }
                            else
                            {
                                // Save current element
                                temp[temp_index] = input[i];
                                temp_index += 1;
                            }
                        }
                        else
                        {
                            // Save first entry (filename)
                            temp[temp_index] = input[i];
                            temp_index += 1;
                        }
                    }
                }
            }
        }
    }
//[================================================================================================]

    else if(version == TAG_3)
    {
        // tag multiple files with a single tag
        
        bool ignore = true;
        bool list = false;
        bool arrow = false;
        bool single = false;

        for(unsigned int i = 0; i < input.length(); i++)
        {
            if(ignore) // Ignore command name and first flag/space/etc
            {
                if(input[i] == '[')
                {
                    ignore = false;
                    list = true;
                    continue;
                }
                else{continue;}
            }
            else if(list) // Write out the list elements
            {
                if(input[i] != ',' && input[i] != ']')
                {
                    temp[temp_index] = input[i];
                    temp_index += 1;
                }
                else if(input[i] == ']')
                {
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                    list = false; // done with list 
                    arrow = true; // start writing flag
                }
                else 
                {
                    // Write out list element
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;
                }
            }
            else if(arrow)
            {
                if(input[i] == ' ') // skip space before flag
                {
                    continue;
                }
                else
                {
                    // Write out flag
                    temp[temp_index] = input[i];
                    temp[temp_index+1] = input[i+1];
                    memcpy(cmnd + offset,temp,max_string_size);
                    memset(temp,'\0',max_string_size);
                    temp_index = 0;
                    offset += max_string_size;

                    // Skip rest of flag + space after flag
                    i += 2;

                    arrow = false; // done with flag
                    single = true; // Write single entry (tagname)
                }
            }
            else if(single)
            {
                // Save tagname
                temp[temp_index] = input[i];
                temp_index += 1;
            }
        }

        // Write tag name
        memcpy(cmnd + offset,temp,max_string_size);
        memset(temp,'\0',max_string_size);
        temp_index = 0;
        offset += max_string_size;
    }
}
//[================================================================================================]








#endif

