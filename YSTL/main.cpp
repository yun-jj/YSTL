#include"include/YSTLVector.h"
#include<string>

using namespace YSTL;

template<typename value>
void testVectorRandomAccess(YVector<value>& ve,std::string information)
{
    for(int i = 0;ve.size() > i;++i)
        std::cout << ve[i] <<" ";
    std::cout << std::endl << information << std::endl;
}

template<typename value>
void testVectorIteratorAccess(YVector<value>& ve,std::string information)
{
    for(auto i = ve.begin();i != ve.end();++i)
        std::cout << *i << " ";
    std::cout <<std::endl << information << std::endl;
}

int main(void)
{
    YVector<std::string> ve1;
    ve1.push_back("123");
    ve1.emplace_back("456");
    testVectorIteratorAccess(ve1,"ve1");
    return 0;
}