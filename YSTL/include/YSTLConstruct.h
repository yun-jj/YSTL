#ifndef YSTLCONSTRUCT
#define YSTLCONSTRUCT

#include<new>
#include"YSTLIterator.h"

//对象的构造与析构的全局函数
namespace YSTL
{
template<typename T,typename value>
inline void
construct(T* p,value& v)
{
    ::new(static_cast<void*>(p)) T(v);
}

template<typename T,typename... value>
inline void
construct(T* p,value&&... v)
{
    ::new(static_cast<void*>(p)) T(std::forward<value>(v)...);
}

template<typename T>
inline void
construct(T* p)
{
    ::new(static_cast<void*>(p)) T;
}

template<typename T>
inline void
destroy(T* p)
{p->~T();}

//模板中的bool值用来做平凡析构函数的优化

template<typename _ForwardIterator>
inline void
destroy(_ForwardIterator first,_ForwardIterator last)
{
    for(;first != last;++first)
        destroy(std::__addressof(*first));
}


template<typename _ForwardIterator,typename sizeType>
inline void
destroy(_ForwardIterator first,sizeType n)
{
    for(;n > 0;++first,--n)
    {
            destroy(std::__addressof(*first));
    }
}

} // namespace YSTL


#endif