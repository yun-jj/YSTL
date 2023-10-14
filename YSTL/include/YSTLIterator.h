#ifndef YSTLITERATOR
#define YSTLITERATOR

#include <cstddef>
#include <type_traits>
# include<cstdlib>

namespace YSTL
{
//五个基本的迭代器类型
struct inputIterator{};
struct outputIterator{};
struct ForwardIterator : public inputIterator{};
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

template<typename Iter>
inline constexpr
typename IteratorTraits<Iter>::categoryType
iteratorCategory(const Iter&)
{return typename IteratorTraits<Iter>::categoryType();}

/*****以下是关于迭代器的函数*****/
//distance 函数用于计算两个迭代器之间的距离
template<typename _RandomAccessIterator>
inline 
typename IteratorTraits<_RandomAccessIterator>::differenceType
_distance(_RandomAccessIterator first,_RandomAccessIterator last,RandomAccessIterator)
{return last - first;}

template<typename _InputIterator>
inline
typename IteratorTraits<_InputIterator>::differenceType
_distance(_InputIterator first,_InputIterator last,inputIterator)
{
    typename IteratorTraits<_InputIterator>::differenceType n = 0;
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
template<typename _InputIterator,typename _differenceType>
inline void
_advance(_InputIterator i,_differenceType n,inputIterator)
{
    while(n--)
        ++i;
}

template<typename _BidirectionalIterator,typename _differenceType>
inline void
_advance(_BidirectionalIterator i,_differenceType n,BidirectionalIterator)
{
    if(n > 0)
        while(n--)
            ++i;
    else
        while(n++)
            --i;
}

template<typename _RandomAccessIterator,typename _differenceType>
inline void
_advance(_RandomAccessIterator i,_differenceType n,RandomAccessIterator)
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

//以下是NormalIterator 运算重载 为什么需要left和right类型的函数重载 源码note如下

// Note: In what follows, the left- and right-hand-side iterators are
// allowed to vary in types (conceptually in cv-qualification) so that
// comparison between cv-qualified and non-cv-qualified iterators be
// valid.  However, the greedy and unfriendly operators in std::rel_ops
// will make overload resolution ambiguous (when in scope) if we don't
// provide overloads whose operands are of the same type.  Can someone
// remind me what generic programming is about? -- Gaby

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator==(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator==(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator!=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() != rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator!=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() != rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator<(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator<(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator>(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() > rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator>(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() > rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator<=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() <= rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator<=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() <= rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr bool
operator>=(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() >= rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr bool
operator>=(const NormalIterator<Iterator,Container>& lIterator,
            const NormalIterator<Iterator,Container>& rIterator)
{return lIterator.base() >= rIterator.base();}

template<typename IteratorLeft,typename IteratorRight,typename Container>
inline constexpr typename NormalIterator<IteratorLeft,Container>::differenceType
operator-(const NormalIterator<IteratorLeft,Container>& lIterator,
            const NormalIterator<IteratorRight,Container>& rIterator)
{return lIterator.base() - rIterator.base();}

template<typename Iterator,typename Container>
inline constexpr NormalIterator<Iterator,Container>
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
    base() const 
    {return current;}

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
};

//Moveiterator 行为与底层迭代器相同
//但其解引用运算符会隐式将底层迭代器的解引用运算符返回值转换为右值引用。
//一些通用算法可以使用移动迭代器来替代复制操作，以实现移动操作。
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
    typedef Iterator                            pointerType;
    //如果baseReferenceType是引用类型 referenceType 的类型为 type&& 否则就为原本的baseReferenceType类型
    typedef typename std::conditional<std::is_reference<baseReferenceType>::value,
        typename std::remove_reference<baseReferenceType>::type&&,baseReferenceType>::type 
    referenceType;

    constexpr
    MoveIterator() 
    : current() {}

    explicit constexpr
    MoveIterator(IteratorType i)
    : current(i) {}

    template<typename Iter>
    constexpr
    MoveIterator(const MoveIterator<Iter>& i)
    : current(i.base()) {}

    constexpr 
    IteratorType
    base() const
    {return current;}

    constexpr 
    referenceType
    operator*() const
    {return static_cast<referenceType>(*current);}

    constexpr
    pointerType
    operator->() const
    {return current;}

    constexpr
    MoveIterator&
    operator++()
    {
        ++current;
        return *this;
    }

    constexpr
    MoveIterator
    operator++(int)
    {
        MoveIterator tmp = *this;
        ++current;
        return tmp;
    }

    constexpr
    MoveIterator&
    operator--()
    {
        --current;
        return *this;
    }

    constexpr
    MoveIterator
    operator--(int)
    {
        MoveIterator tmp = *this;
        --current;
        return tmp;
    }

    constexpr
    MoveIterator
    operator+(differenceType n)
    {return MoveIterator(current + n);}

    constexpr
    MoveIterator&
    operator+=(differenceType n)
    {
        current += n;
        return *this;
    }

    constexpr
    MoveIterator
    operator-(differenceType n)
    {return MoveIterator(current - n);}

    constexpr
    MoveIterator&
    operator-=(differenceType n)
    {
        current -= n;
        return *this;
    }

    constexpr
    referenceType
    operator[](differenceType n) const
    {return std::move(current[n]);}
};

template<typename Iterator>
inline constexpr 
MoveIterator<Iterator>
makeMoveIterator(Iterator i)
{return MoveIterator<Iterator>(i);}

// template<typename Inter>
//     using RequireForwardIter = typename 
//         std::enable_if<std::is_convertible<typename
// 		IteratorTraits<InIter>::categoryType,
// 			       ForwardIterator>::value>::type;

//以下是MoveIterator的运算符重载

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator==(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename Iterator>
inline constexpr bool
operator==(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return lIterator.base() == rIterator.base();}

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator!=(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return !(lIterator == rIterator);}

template<typename Iterator>
inline constexpr bool
operator!=(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return !(lIterator == rIterator);}

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator<(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename Iterator>
inline constexpr bool
operator<(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return lIterator.base() < rIterator.base();}

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator<=(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return !(rIterator < lIterator);}

template<typename Iterator>
inline constexpr bool
operator<=(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return !(rIterator < lIterator);}

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator>(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return rIterator < lIterator;}

template<typename Iterator>
inline constexpr bool
operator>(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return rIterator < lIterator;}

template<typename IteratorLeft,typename IteratorRight>
inline constexpr bool
operator>=(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
{return !(lIterator < rIterator);}

template<typename Iterator>
inline constexpr bool
operator>=(const MoveIterator<Iterator>& lIterator,
            const MoveIterator<Iterator>& rIterator)
{return !(lIterator < rIterator);}

//DR685
template<typename IteratorLeft,typename IteratorRight>
inline constexpr auto
operator-(const MoveIterator<IteratorLeft>& lIterator,
            const MoveIterator<IteratorRight>& rIterator)
->decltype(lIterator.base() - rIterator.base())
{return lIterator.base() - rIterator.base();}

template<typename Iterator>
inline constexpr MoveIterator<Iterator>
operator+(typename MoveIterator<Iterator>::differenceType n,
            const MoveIterator<Iterator>& x)
{return x + n;}

} // namespace YSTL

    
#endif