#include<iostream>
#include<cstdint>
using namespace std;

typedef uint8_t BClass;

class B {
	public:
		void begin() { cout<<" begun B!\n"; }
	};

class A {
	public:
		int mass = 23;
		void begin(){ cout<<"begun!\n";}
		A(int cmass = 24, BClass *theA = nullptr);
		
	private:
		BClass  *_BAobj = nullptr;
		uint8_t *_UAobj = nullptr;
};

A::A(int cmass, BClass *theA){
	mass = cmass;
	_BAobj = theA;
	_UAobj = theA;
}

int main() {
	B set;	
	A *ptr = new A(34, &set);
	
	ptr->begin();
	delete ptr;

	return 0;
}
