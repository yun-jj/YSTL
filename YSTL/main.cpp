#include"include/YSTLVector.h"

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
    //值初始化
    YVector<int> ve(10,2);
    ve.data();
    ve.reserve(11);
    testVectorRandomAccess(ve,"ve");

    YVector<int> ve1(ve.begin(),ve.end());
    testVectorRandomAccess(ve1,"ve1");

    ve[2] = 3;
    ve.swap(ve1);
    ve.push_back(4);
    ve.insert(ve.begin(),10);
    testVectorIteratorAccess(ve,"ve");
    ve.insert(ve.begin(),1,8);
    testVectorIteratorAccess(ve,"ve");
    ve.clear();
    return 0;
}