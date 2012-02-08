#include <iostream>
#include <typeinfo>


using namespace std;


class CBase
{
	public:
		virtual ~CBase(){}
};

class CSub1 : public CBase
{
	
};
class CSub2 : public CBase
{

};

void func(int a)
{
	
}
int ffunc( CSub2 a )
{
	return 0;
}
CSub2 *fffunc( CSub1 &a )
{
	return 0;
}
int main()
{
	CBase base_obj;
	CSub1 sub1_obj;
	CSub2 sub2_obj;
	
//	CBase *p = &base_obj;
  CBase *p = &sub1_obj;
//  CBase *p = &sub2_obj;

//	type_info  info = typeid( *p );  Not working...
	if( typeid(*p) == typeid( CBase ) ){
		std::cout << "CBase" << std::endl;
	} else if( typeid(*p) == typeid( CSub1 ) ){
		std::cout << "CSub1" << std::endl;
	} else if( typeid(*p) == typeid( CSub2 ) ){
		std::cout << "CSub2" << std::endl;
	}
	std::cout << "Test:" << typeid(typeid(*p)).name() << std::endl;
	std::cout << "Test:" << typeid(1L).name() << std::endl;	
	std::cout << "Test:" << typeid(func).name() << std::endl;		
	std::cout << "Test:" << typeid(ffunc).name() << std::endl;			
	std::cout << "Test:" << typeid(fffunc).name() << std::endl;				
	return 0;
}