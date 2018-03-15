

#include "Parser.h"

int main()
{
    std::string test1 = "find -t [adhds,aadgsdb,csdf,dsd,kdfh,ldis,mjjj,tdkd,oad,edFD,jFDF]";
    std::string test2 = "find -t {a,b,c,s,e,f,g,h,j,l,k,p,o}";
    std::string test3 = "find -t [a,b,c,{d,f,g},l,r,{t,k}]";
    std::string test4 = "find -t {a,b,c,[gk,lm,rrr],t,j,[y,u],l,k,o,p,r,[q,d]}";
    std::string test5 = "find -t [a,b,c,{g,j,k,[l,m,n],o,p,q,[r,s]},t,u,v]";
    std::string test6 = "find -f [file.txt,file2.txt,file3.txt]";
    std::string test7a = "new -t [tag1,tag2,tag3,tag4]";
    std::string test7b = "new -f [file.txt,file2.txt,file3.txt]";
    std::string test8 = "new -f file.txt -t [tag1,tag2,tag3]";
    std::string test9 = "delete -t [tag1,tag2,tag3]";
    std::string test10 = "delete -f [file.txt,file2.txt,file3.txt]";
    std::string test11 = "fdelete -t [tag1]";
    std::string test12 = "open /tag1/tag2/tag3/file.txt";
    std::string test13 = "close /tag1/tag2/tag3/file2.txt";
    std::string test14a = "rename -t [tag1,tag2,tag3] -n [tag4,tag5,tag6]";
    std::string test14b = "rename -t [tag1,tag2] -n [tag4,tag5,tag6]";
    std::string test14c = "rename -t [tag1,tag2,tag3] -n [tag4,tag5]";
    std::string test15a = "rename -f [file.txt,file1.txt,file2.txt] -n [file3.txt,file4.pdf]";
    std::string test15b = "rename -f [file.txt,file1.txt,file2.txt] -n [file3.txt,file4.pdf,file5.png]";
    std::string test15c = "rename -f [file.txt,file1.txt] -n [file3.txt,file4.pdf,file5.png]";
    std::string test16 = "attr [file1.txt,file2.txt]";
    std::string test17 = "merge tag1 -> tag99";
    std::string test18 = "merge [tag1,tag2,tag3,tag4] -> tag99";
    std::string test19 = "tag file.txt -> [tag1,tag2,tag3,tag4]";
    std::string test19b = "tag file.txt -> [tag1]";
    std::string test20 = "tag [file.txt,file1.txt,file2.txt] -> tag99";
    std::string cwd_test = "/t1/t2/t3/t4/t5";

    Parser p(test1,"/",64);
    std::vector<std::string> vec = p.parse(4);

    std::cout << "\nTest 4\n";
    for(uint i = 0; i < vec.size(); i++)
    {
      std::cout << "Item: " << vec[i] << std::endl;
    }
    //Potentially use an In-place algorithm
    p.reset(test2);

    vec = p.parse(4);
    std::cout << "\nTest 4\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test3);

    vec = p.parse(4);
    std::cout << "\nTest 4\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test4);

    vec = p.parse(4);
    std::cout << "\nTest 4\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test5);

    vec = p.parse(4);
    std::cout << "\nTest 4\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test6);
    vec = p.parse(5);
    std::cout << "\nTest 5\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test7a);
    vec = p.parse(6);
    std::cout << "\nTest 6\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test7b);
    vec = p.parse(7);
    std::cout << "\nTest 7\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test8);
    vec = p.parse(8);
    std::cout << "\nTest 8\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test9);
    vec = p.parse(9);
    std::cout << "\nTest 9\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test10);
    vec = p.parse(10);
    std::cout << "\nTest 10\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test11);
    vec = p.parse(11);
    std::cout << "\nTest 11\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test12);
    vec = p.parse(12);
    std::cout << "\nTest 12\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }


    p.reset(test13);
    vec = p.parse(13);
    std::cout << "\nTest 13\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }


    p.reset(test14a);
    vec = p.parse(14);
    std::cout << "\nTest 14A\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test14b);
    vec = p.parse(14);
    std::cout << "\nTest 14B\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test14c);
    vec = p.parse(14);
    std::cout << "\nTest 14C\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test15a);
    vec = p.parse(15);
    std::cout << "\nTest 15A\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test15b);
    vec = p.parse(15);
    std::cout << "\nTest 15B\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test15c);
    vec = p.parse(15);
    std::cout << "\nTest 15C\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test16);
    vec = p.parse(16);
    std::cout << "\nTest 16\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test17);
    vec = p.parse(17);
    std::cout << "\nTest 17\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test18);
    vec = p.parse(18);
    std::cout << "\nTest 18\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test19);
    vec = p.parse(19);
    std::cout << "\nTest 19\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test19b);
    vec = p.parse(19);
    std::cout << "\nTest 19B\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    p.reset(test20);
    vec = p.parse(20);
    std::cout << "\nTest 20\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    vec = p.get_cwd_tags();
    std::cout << "\nTest CWD\n";
    for(uint i = 0; i < vec.size(); i++)
    {
        std::cout << "Item: " << vec[i] << std::endl;
    }

    return 0;
}
