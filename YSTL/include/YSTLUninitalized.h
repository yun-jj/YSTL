#ifndef YSTLUNINITALIZED
#define YSTLUNINITALIZED

#include<memory>

#include"YSTLConstruct.h"

namespace YSTL
{
template<typename _InputIterator,typename _ForwardIterator>
_ForwardIterator
uninitializedCopy(_InputIterator first,_InputIterator last,_ForwardIterator result)
{
    _ForwardIterator cur = result;
    for(;first != last;++cur,++first)
        construct(std::__addressof(*cur),*first);
    return cur;
}

template<typename _ForwardIterator,typename value>
void
uninitializedFill(_ForwardIterator first,_ForwardIterator last,const value& v)
{
    for(;first != last;++first)
        construct(std::__addressof(*first),v);
}

template<typename _ForwardIterator,typename sizeType,typename value>
_ForwardIterator
uninitializedFillN(_ForwardIterator first,sizeType n,value v)
{
    _ForwardIterator cur = first;
    for(;n > 0;--n,++cur)
        construct(std::__addressof(*cur),v);
    return cur;
}
};


template<typename _ForwardIterator,typename sizeType>
_ForwardIterator
uninitializedDefaultNA(_ForwardIterator first,sizeType n)
{
    _ForwardIterator cur = first;
    for(;n > 0;--n,++cur)
        construct(std::__addressof(*cur));
    return cur;
}


#endif