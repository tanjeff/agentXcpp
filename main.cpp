#include <iostream>
#include <fstream>
#include "oid.h"
#include "Integer.h"
#include "octet_string.h"
#include "varbind.h"
#include "Counter32.h"
#include "Gauge32.h"


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
    cout << endl;
}


void test_octet_string()
{
    cout << "--- Testing octet_string ---" << endl;

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

    //octet_string os2;
    //os2.deserialize(data.begin(), true);
    //cout << "os2 is \"" << os2 << "\"" << endl;

}



class myinteger : public Integer
{
    public:
	myinteger(int v) { value = v; }
	myinteger() {}
	friend std::ostream& operator<<(std::ostream&, const myinteger&);

};
std::ostream& operator<<(std::ostream& out, const myinteger& i)
{
    out << i.value;

    return out;
}
void test_integer()
{
    cout << "--- Testing Integer ---" << endl;

    myinteger i1(0xcafebabe);
    cout << hex;
    cout << "Integer value is " << i1 << endl;

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

    myinteger i2;
    i2.deserialize(data.begin(), true);
    cout << "i2 is " << i2 << endl;

    cout << dec;

}




class mycounter32 : public Counter32
{
    public:
	mycounter32(int v) { value = v; }
	mycounter32() {}
	friend std::ostream& operator<<(std::ostream&, const mycounter32&);

};
std::ostream& operator<<(std::ostream& out, const mycounter32& i)
{
    out << i.value;

    return out;
}
void test_counter32()
{
    cout << "--- Testing Counter32 ---" << endl;

    mycounter32 c32_1(0xcafebabe);
    cout << hex;
    cout << "Counter32 value is " << c32_1 << endl;

    data_t data;

    ofstream ofile("c32_1.oid");
    data = c32_1.serialize();
    print_serialized(data);
    for( int i = 0; i < data.size(); i++)
    {
        ofile.put((char)data[i]);
    }
    cout << endl;
    ofile.close();


    data.clear();
    
    ifstream ifile("c32_1.oid");
    char ch;
    while ( ifile.get(ch) )
    {
        data.push_back(ch);
    }
    ifile.close();
    cout << "Read " << data.size() << " bytes." << endl;

    mycounter32 c32_2;
    c32_2.deserialize(data.begin(), true);
    cout << "c32_2 is " << c32_2 << endl;

    cout << dec;

}




class myGauge32 : public Gauge32
{
    public:
	myGauge32(int v) { value = v; }
	myGauge32() {}
	friend std::ostream& operator<<(std::ostream&, const myGauge32&);

};
std::ostream& operator<<(std::ostream& out, const myGauge32& i)
{
    out << i.value;

    return out;
}
void test_Gauge32()
{
    cout << "--- Testing Gauge32---" << endl;

    myGauge32 g1(0xcafebabe);
    cout << hex;
    cout << "Gauge32 value is " << g1 << endl;

    data_t data;

    ofstream ofile("Gauge32_1.oid");
    data = g1.serialize();
    print_serialized(data);
    for( int i = 0; i < data.size(); i++)
    {
        ofile.put((char)data[i]);
    }
    cout << endl;
    ofile.close();


    data.clear();
    
    ifstream ifile("Gauge32_1.oid");
    char ch;
    while ( ifile.get(ch) )
    {
        data.push_back(ch);
    }
    ifile.close();
    cout << "Read " << data.size() << " bytes." << endl;

    myGauge32 g2;
    g2.deserialize(data.begin(), true);
    cout << "g2 is " << g2 << endl;

    cout << dec;

}

void test_varbind()
{
    cout << "--- Testing varbind ---" << endl;

    data_t data;
    
    oid o(1,3,6,1,23,42);
    
    myinteger i(0xa5);
    varbind vb_integer(&o, &i);
    data = vb_integer.serialize();
    print_serialized(data);
    
    data_t tmp;
    tmp.push_back('H');
    tmp.push_back('e');
    tmp.push_back('l');
    tmp.push_back('l');
    tmp.push_back('o');
    octet_string os(tmp);
    varbind vb_octet_string(&o, &os);
    data = vb_octet_string.serialize();
    print_serialized(data);
    
    mycounter32 c32(0x5a);
    varbind vb_counter32(&o, &c32);
    data = vb_counter32.serialize();
    print_serialized(data);
    
    myGauge32 g32(0x5a);
    varbind vb_Gauge32(&o, &g32);
    data = vb_Gauge32.serialize();
    print_serialized(data);
    
    cout << endl;
}

int main()
{
    test_integer();
    test_octet_string();
    test_counter32();
    test_Gauge32();


    test_varbind();

    return 0;
}
