#include "Parser.hpp"

// Use tutorials in: https://llvm.org/docs/tutorial/
#include <iostream>
#include <fstream>
using namespace std;
int main (int argc, char *argv[])
{
    /*ifstream is(argv[1]);
    if(!is){
        cout << argv[1] << endl;
        cout<< " failed " << endl;
    }*/
    Parser parser(cin);

    if (!parser.Parse()) {
        return 1;
    }
    parser.Generate().print(llvm::outs(), nullptr);

    return 0;
}
