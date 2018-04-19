// Danny Radosevich
// Senior Desgin spring 2018
// Ideally, the go between between javascript and cpp
#include <iostream>
using namespace std;
#include <string>
using std:: string;
#include <vector>
using std::vector;

//string translator(string toSplit);
//vector<string> splitter(string str);
//string assembler(vector<string> inVec);

/*
This function will imput a string from java script(I guess),
convert to a vector to interact with adrians function, and take the return
vector, convert it back to a string for javascript
the java script will parse the string into something it can deal with,
rather than a vector
*/
extern "C"
{
  //function to reassemble a string froma  vector
  string assembler(vector<string> inVec)
  {
    string toReturn = "";
    for (unsigned int i = 0; i < inVec.size(); i++)
    {
      toReturn += inVec[i];
    }
    return toReturn;
  }
  //function to split the string into a vector
  vector<string> splitter(string str)
  {
    vector<string> splitVec;
    string delim = "//";
    size_t pos = 0;
    string token;
    while((pos = str.find(delim))!= std::string::npos)//internet black magic
    {
      token = str.substr(0, pos);
      splitVec.push_back(token);
      str.erase(0, pos + delim.length());
    }
    return splitVec;
  }
string translator(string toSplit)
{
  string toReturn;
  vector<string> splitted;
  vector<string> reassemble;
  splitted = splitter(toSplit);
  //code to call to adrians stuffs
  //reassemble = function //get the return vector from adrians stuff
  toReturn = assembler(reassemble);

}

}
