
#include <iostream>
using namespace std;

struct S
	{
	char a[10];
	void suck(){}
	S(){}
	~S(){}
	static int x()	{	long a=3+4+5+6+7;	a+=8;	}
	char f[10];
	};

int main()
	{
	S s;
	cout<<"Size of S: "<<sizeof(S)<<endl;
	cout<<"Size of s: "<<sizeof(s)<<endl;
	cout<<"dist from a to end of f: "<<((long)(&s.f)-(long)(&s.a)+sizeof(s.f))<<endl;
	}
