#include <iostream>
#include <fstream>
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
    basic_string<uint8_t> data;

    ofstream o1_file("o1.oid");
    oid o1(1,2,6,1,5,6,7,8,9);
    data = o1.serialize(0xee);
    print_serialized(data);
    for( int i = 0; i < data.size(); i++)
    {
	o1_file.put((char)data[i]);
    }
    cout << endl;
    o1_file.close();


    data.clear();
    
    ifstream file("o1.oid");
    char ch;
    while ( file.get(ch) )
    {
	data.push_back(ch);
    }
    file.close();
    cout << "Read " << data.size() << " bytes." << endl;

    oid o2;
    o2.deserialize(data);
    cout << "o2 is " << o2 << endl;

    return 0;
}
