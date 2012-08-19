#include <iostream>

using namespace std;

int main()
	{
	int a;
	cout<<"enter number: ";
	cin>>a;
	int slot;
	cout<<"enter xor slot: ";
	cin>>slot;
	int bitmask=1<<slot;
	int b=a;
	b^=bitmask;
	cout<<a<<" XOR "<<bitmask<<" = "<<b<<endl;
	int c=a;
	c=c&(~bitmask);
	cout<<a<<" AND (NOT "<<bitmask<<") = "<<c<<endl;
	}
