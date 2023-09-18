#ifndef YSTLITERATOR
#define YSTLITERATOR

#include <cstddef>
#include <type_traits>

namespace YSTL
{
//三个基本的迭代器类型
struct ForwardIterator{};
struct BidirectionalIterator : public ForwardIterator{};
struct RandomAccessIterator : public BidirectionalIterator{};

//迭代器特性萃取器
template<typename Iterator>
struct IteratorTraits
{
    typedef typename Iterator::valueType         valueType; 
    typedef typename Iterator::categoryType      categoryType; 
    typedef typename Iterator::pointerType       pointerType; 
    typedef typename Iterator::referenceType     referenceType; 
    typedef typename Iterator::differenceType    differenceType; 
}; 

template<typename T>
struct IteratorTraits<T*>
{
    typedef T                       valueType;
    typedef RandomAccessIterator    categoryType;
    typedef T*                      pointerType;
    typedef T&                      referenceType;
    typedef ptrdiff_t               differenceType;    
};

template<typename T>
struct IteratorTraits<const T*>
{
    typedef T                       valueType;
    typedef RandomAccessIterator    categoryType;
    typedef const T*                pointerType;
    typedef const T&                referenceType;
    typedef ptrdiff_t               differenceType;
};

//基本迭代器结构体定义了基本的属性
template<typename value,typename category>
struct Iterator
{
    typedef value       valueType;
    typedef value*      pointerType;
    typedef value&      referenceType;
    typedef ptrdiff_t   differenceType;
};
/*****以下是关于迭代器的函数*****/
//distance 函数用于计算两个迭代器之间的距离
template<typename _RandomAccessIterator>
inline 
typename IteratorTraits<_RandomAccessIterator>::differenceType
_distance(_RandomAccessIterator first,_RandomAccessIterator last,RandomAccessIterator)
{return last - first;}

template<typename _ForwardIterator>
inline
typename IteratorTraits<_ForwardIterator>::differenceType
_distance(_ForwardIterator first,_ForwardIterator last,ForwardIterator)
{
    typename IteratorTraits<_ForwardIterator>::differenceType n = 0;
    for(;first != last;++first)
        ++n;
    return n;
}

template<typename _Iterator>
inline
typename IteratorTraits<_Iterator>::differenceType
distance(_Iterator first,_Iterator last)
{
    return _distance(first,last,typename IteratorTraits<_Iterator>::categoryType());
}
//advance 函数用于移动迭代器
template<typename _Iterator,typename _differenceType>
inline void
_advance(_Iterator i,_differenceType n,ForwardIterator)
{
    while(n--)
        ++i;
}

template<typename _Iterator,typename _differenceType>
inline void
_advance(_Iterator i,_differenceType n,BidirectionalIterator)
{
    if(n > 0)
        while(n--)
            ++i;
    else
        while(n++)
            --i;
}

template<typename _Iterator,typename _differenceType>
inline void
_advance(_Iterator i,_differenceType n,RandomAccessIterator)
{i+=n;}

template<typename _Iterator,typename _n>
inline void
advance(_Iterator i,_n n)
{
    _advance(i,n,typename IteratorTraits<_Iterator>::categoryType());
}
/*****以下是迭代器适配器作为最终的迭代器类型*****/
//基本迭代器类型
template<typename Iterator,typename Container>
class NormalIterator
{
protected:
    Iterator current;
public:
    typedef Iterator                             IteratorType;
    typedef IteratorTraits<IteratorType>         traitsType;
    typedef typename traitsType::categoryType    categoryType;
    typedef typename traitsType::valueType       valueType;
    typedef typename traitsType::differenceType  differenceType;
    typedef typename traitsType::referenceType   referenceType;
    typedef typename traitsType::pointerType     pointerType;

    constexpr NormalIterator() noexcept : current(Iterator()){}
    constexpr NormalIterator(const Iterator& i) noexcept : current(i) {}
    //下面是将iterator转换为constIterator的构造函数
    template<typename iter>
    NormalIterator(const NormalIterator<iter,typename
    std::enable_if<(std::is_same<iter,typename Container::pointerType>::value),Container>::type>& i)noexcept
        :current(i.base()) {}



    referenceType
    operator*() const noexcept
    {return *current;}

    pointerType
    operator->() const noexcept
    {return current;}

    NormalIterator&
    operator++() noexcept
    {
        ++current;
        return *this;
    }

    NormalIterator&
    operator--() noexcept
    {
        --current;
        return *this;
    }

    NormalIterator
    operator++(int) noexcept
    {return NormalIterator(current++);}

    NormalIterator
    operator--(int) noexcept
    {return NormalIterator(current--);}

    referenceType
    operator[](differenceType n) const noexcept
    {return current[n];}

    NormalIterator&
    operator+=(differenceType n) const noexcept
    {current += n; return *this;}

    NormalIterator&
    operator-=(differenceType n) const noexcept
    {current -= n; return *this;}

    NormalIterator
    operator+(differenceType n) noexcept
    {return NormalIterator(current + n);}

    NormalIterator
    operator-(differenceType n) noexcept
    {return NormalIterator(current - n);}

    const Iterator&
    base() const noexcept
    {return current;}
};

//以下是NormalIterator 运算重载
template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator==(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator==(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator!=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() != rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator!=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() != rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator<(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator<(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator>(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() > rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator>(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() > rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator<=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() <= rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator<=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() <= rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline bool
operator>=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() >= rIterator.base();}

template<typename Iterator,typename Container>
inline bool
operator>=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() >= rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline typename NormalIterator<IteratorLeft,Container>::differenceType
operator-(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() - rIterator.base();}

template<typename Iterator,typename Container>
inline NormalIterator<Iterator,Container>
operator+(typename NormalIterator<Iterator,Container>::differenceType n,
            const NormalIterator<Iterator,Container>& iterator)
{return NormalIterator<Iterator,Container>(iterator.base() + n);}


//反转的迭代器在移动上与一般迭代器相反
template<typename Iterator>
class ReverseIterator : Iterator
{
protected:
    Iterator current;
    typedef IteratorTraits<Iterator> traitsType;
public:
    typedef Iterator                             IteratorType;
    typedef typename traitsType::differenceType  differenceType;
    typedef typename traitsType::pointerType     pointerType;
    typedef typename traitsType::referenceType   referenceType;

    constexpr
    ReverseIterator()noexcept : current() {}

    explicit constexpr
    ReverseIterator(IteratorType i) : current(i) {}

    constexpr
    ReverseIterator(const ReverseIterator& i) : current(i.current) {}

    constexpr
    IteratorType
    base() {return current;}

    constexpr
    referenceType
    operator*() const
    {
        Iterator tmp = current;
        return *--tmp;
    }

    constexpr
    pointerType
    operator->() const
    {return &(operator*());}

    constexpr
    ReverseIterator&
    operator++()
    {
        --current;
        return *this;
    }

    constexpr
    ReverseIterator&
    operator--()
    {
        ++current;
        return *this;
    }

    constexpr
    ReverseIterator
    operator++(int)
    {
        ReverseIterator tmp = *this;
        --current;
        return tmp;
    }

    constexpr
    ReverseIterator
    operator--(int)
    {
        ReverseIterator tmp = *this;
        ++current;
        return tmp;
    }

    constexpr
    ReverseIterator
    operator+(differenceType n)
    {
        return ReverseIterator(current - n);
    }

    constexpr
    ReverseIterator
    operator-(differenceType n)
    {
        return ReverseIterator(current + n);
    }

    constexpr
    ReverseIterator&
    operator+=(differenceType n)
    {
        current -= n;
        return *this;
    }

    constexpr
    ReverseIterator&
    operator-=(differenceType n)
    {
        current += n;
        return *this;
    }


template<typename Iterator>
class MoveIterator
{
protected:
    Iterator current;
    
    typedef IteratorTraits<Iterator>            traitsType;
    typedef typename traitsType::referenceType  baseReferenceType;
public:
    typedef Iterator IteratorType;
    typedef typename traitsType::categoryType   categoryType;
    typedef typename traitsType::valueType      valueType;
    typedef typename traitsType::differenceType differenceType;
};

};

} // namespace YSTL

    
#endif