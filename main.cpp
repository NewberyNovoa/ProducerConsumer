#include <iostream> 
#include "ItemProcessor.h"

using namespace std;

int main(int argc, char** argv)
{

    ItemProcessor ip (argv[1], argv[2], argv[3]);

    if(ip.start())
    	cout<<"Done.."<<endl;
    else
    	cout<<"Error"<<endl;

    return 0;
}