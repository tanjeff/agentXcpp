#include <iostream>
#include "oid.h"


using namespace agentx;
using namespace std;


void print_serialized(const basic_string<uint8_t> stream)
{

    cout << "+----------+----------+----------+----------+" << endl;
    cout << "| ";//begin line
    for(int i = 0; i < stream.size(); i++)
    {
	cout.width(8);// 8 chars per field
	cout << (int)stream[i] << " | ";
	if( (i+1)%4 == 0 )
	{
	    cout << endl;   // end line
	    cout << "+----------+----------+----------+----------+";
	    if( i != stream.size() - 1 )
	    {
		// We have further data; begin a new line
		cout << endl << "| ";
	    }
	}
    }
}

int main()
{
    oid o1(1,2,3,4,5,6,7,8,9);
    print_serialized(o1.serialize(0xee));
    cout << endl;
    
    oid o2(1,3,6,1,5,6,7,8,9);
    print_serialized(o2.serialize(0xee));
    cout << endl;

    oid o3(1,3,6,1,2);
    print_serialized(o3.serialize(0xee));
    cout << endl;

    oid o4(1);
    print_serialized(o4.serialize(0xee));
    cout << endl;

    oid o5(1);
    print_serialized(o5.serialize(0));

    return 0;
}
