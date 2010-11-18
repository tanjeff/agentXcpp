#include <iostream>
#include <fstream>
#include "oid.h"
#include "integer.h"
#include "octet_string.h"


using namespace agentx;
using namespace std;


void print_serialized(data_t stream)
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


void test_octet_string()
{
    data_t tmp;
    tmp.push_back('H');
    tmp.push_back('e');
    tmp.push_back('l');
    tmp.push_back('l');
    tmp.push_back('o');

    octet_string os1(tmp);

    data_t data;

    ofstream ofile("os1.oid");
    data = os1.serialize();
    print_serialized(data);
    for( int i = 0; i < data.size(); i++)
    {
        ofile.put((char)data[i]);
    }
    cout << endl;
    ofile.close();


    data.clear();
    
    ifstream ifile("os1.oid");
    char ch;
    while ( ifile.get(ch) )
    {
        data.push_back(ch);
    }
    ifile.close();
    cout << "Read " << data.size() << " bytes." << endl;

    integer os2;
    os2.deserialize(data.begin(), true);
    cout << "os2 is \"" << os2 << "\"" << endl;

}
void test_integer()
{

    integer i1(0xcafebabe);
    cout << hex;
    cout << "integer value is " << i1 << endl;

    data_t data;

    ofstream i1_file("i1.oid");
    data = i1.serialize();
    print_serialized(data);
    for( int i = 0; i < data.size(); i++)
    {
        i1_file.put((char)data[i]);
    }
    cout << endl;
    i1_file.close();


    data.clear();
    
    ifstream file("i1.oid");
    char ch;
    while ( file.get(ch) )
    {
        data.push_back(ch);
    }
    file.close();
    cout << "Read " << data.size() << " bytes." << endl;

    integer i2;
    i2.deserialize(data.begin(), true);
    cout << "i2 is " << i2 << endl;

    cout << dec;

}

int main()
{
    test_integer();
    test_octet_string();

    return 0;
}
