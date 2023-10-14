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
    YVector<int> ve1(YVector<int>(10,2));
    ve1[0] = 3;
    ve1.erase(ve1.begin());
    ve1[0] = 1;
    ve1[1] = 2;
    ve1[2] = 3;
    ve1.erase(ve1.begin(),ve1.begin() + 3);
    ve1.emplace(ve1.end(),10);
    ve1.emplace(ve1.begin() + 2,12);
    ve1.emplace_back(77);
    ve1.push_back(1111);
    int tmp = 132;
    ve1.push_back(tmp);
    testVectorIteratorAccess(ve1,"ve1");
    return 0;
}