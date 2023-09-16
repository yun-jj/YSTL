#ifndef YSTLUNINITALIZED
#define YSTLUNINITALIZED

#include<memory>

#include"YSTLConstruct.h"

namespace YSTL
{
template<typename inputIterator,typename ForwardIterator>
ForwardIterator
uninitializedCopy(inputIterator first,inputIterator last,ForwardIterator result)
{
    ForwardIterator cur = result;
    for(;first != last;++cur,++first)
        construct(std::__addressof(*cur),*first);
    return cur;
}

template<typename ForwardIterator,typename value>
void
uninitializedFill(ForwardIterator first,ForwardIterator last,const value& v)
{
    for(;first != last;++first)
        construct(std::__addressof(*first),v);
}

template<typename ForwardIterator,typename sizeType,typename value>
ForwardIterator
uninitializedFillN(ForwardIterator first,sizeType n,value v)
{
    ForwardIterator cur = first;
    for(;n > 0;--n,++cur)
        construct(std::__addressof(*cur),v);
    return cur;
}
};


template<typename ForwardIterator,typename sizeType>
ForwardIterator
uninitializedDefaultNA(ForwardIterator first,sizeType n)
{
    ForwardIterator cur = first;
    for(;n > 0;--n,++cur)
        construct(std::__addressof(*cur));
    return cur;
}


#endif