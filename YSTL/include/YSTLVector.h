#ifndef YSTLVECTOR
#define YSTLVECTOR

#include <algorithm>
#include <type_traits>
#include <utility>

#include "YSTLAllocator.h"
#include "YSTLIterator.h"
#include "YSTLUninitalized.h"

namespace YSTL
{

struct true_type{};
struct false_type{};

template<typename Input>
struct isInteger
{using value = false_type;};

template<>
struct isInteger<int>
{using value = true_type;};

//vector的基本类型 持有容器基本的三个指针    
template<typename T,typename Alloc>
struct YVectorBase
{
    typedef newAllocator<T> allocatorTyep;
    typedef typename Alloc::pointerType pointerType;

    struct YVectorImpl : public allocatorTyep
    {
        pointerType start;
        pointerType finish;
        pointerType endOfStorage;

        YVectorImpl() : allocatorTyep(),start(),finish(),endOfStorage() {}

        YVectorImpl(const allocatorTyep& other) : allocatorTyep(other),start(),finish(),endOfStorage() {}
        //交换容器拥有的对象
        void swapData(YVectorImpl& other)
        {
            std::swap(start,other.start);
            std::swap(finish,other.finish);
            std::swap(endOfStorage,other.endOfStorage);
        }
    };

    YVectorImpl impl;

    YVectorBase() : impl() {}

    YVectorBase(size_t n) : impl()
    {createStorage(n);}

    YVectorBase(YVectorBase&& other) noexcept : impl()
    {this->impl.swapData(other.impl);}

    ~YVectorBase()
    {this->mDeallocator(impl.start,impl.endOfStorage - impl.start);}

    pointerType 
    mAllocator(size_t n)
    {return n > 0 ? impl.allocator(n) : pointerType();}

    void
    mDeallocator(pointerType p,size_t n)
    {
        if(p)
            impl.deallocator(p);
    }

private:
    //创建要求大小的地址
    void createStorage(size_t n)
    {
        this->impl.start = this->mAllocator(n);
        this->impl.finish = this->impl.start;
        this->impl.endOfStorage = this->impl.start + n;
    }
};

template<typename T,typename Alloc = newAllocator<T>>
class YVector : protected YVectorBase<T,Alloc>
{
    typedef Alloc                   AllocType;
    typedef YVectorBase<T,Alloc>    base;

    using base::mAllocator;
    using base::mDeallocator;
    using base::impl;

public:
    typedef T                                                    valueType;
    typedef typename base::pointerType                           pointerType;
    typedef typename Alloc::constPointerType                     constPointerType;
    typedef typename Alloc::referenceType                        referenceType;
    typedef typename Alloc::constReferenceType                   constReferenceType;
    typedef NormalIterator<pointerType,YVector>                  iterator;
    typedef NormalIterator<constPointerType,YVector>             constIterator;
    typedef ReverseIterator<iterator>                            _ReverseIterator;
    typedef ReverseIterator<constIterator>                       _constReverseIterator;
    typedef size_t                                               sizeType;
    typedef ptrdiff_t                                            differenceType;

    YVector() : base() {}

    explicit
    YVector(sizeType n) : base(n) 
    {defaultInitialize(n);}

    YVector(sizeType n,const valueType& value) : base(n)
    {fillInitialize(n,value);}

    YVector(const YVector& other) : base(other.size())
    {this->impl.finish = uninitializedCopy(other.begin(),other.end(),this->impl.start);}

    YVector(YVector&& other) noexcept : base(std::move(other)) {}

    template<typename _InputIterator>
    YVector(_InputIterator first,_InputIterator last)
    {
        typedef typename isInteger<_InputIterator>::value  Integral;
        initializeDispatch(first,last,Integral());
    }

    ~YVector()
    {AllocType::destroyN(impl.start,impl.finish - impl.start);}

    iterator
    begin() noexcept
    {return iterator(this->impl.start);}

    constIterator
    begin() const noexcept
    {return constIterator(this->impl.start);}

    _ReverseIterator
    rbegin() noexcept
    {return _ReverseIterator(this->impl.start);}

    _constReverseIterator
    rbegin() const
    {return _constReverseIterator(this->impl.start);}

    _ReverseIterator
    rend() noexcept
    {return _ReverseIterator(end());}

    _constReverseIterator
    rend() const noexcept
    {return _constReverseIterator(end());}

    iterator
    end() noexcept
    {return iterator(this->impl.finish);}

    constIterator
    end() const noexcept
    {return constIterator(this->impl.finish);}

    constIterator
    cbegin() const noexcept
    {return constIterator(this->impl.start);}

    constIterator
    cend() const noexcept
    {return constIterator(this->impl.finish);}

    constReferenceType
    crbegin() const noexcept
    {return constReferenceType(this->impl.start);}

    constReferenceType
    crend() const noexcept
    {return constReferenceType(this->impl.finish);}

    sizeType
    size() const
    {return sizeType(this->impl.finish - this->impl.start);}

    sizeType
    maxSize()
    {return AllocType::maxSize();}

    referenceType
    operator[](sizeType n)
    {return *(this->impl.start + n);}

    constReferenceType
    operator[](sizeType n) const
    {return *(this->impl.start + n);}

    //输出容器可以使用的空间
    sizeType
    capacity()
    {return sizeType(this->impl.endOfStorage - this->impl.start);}

    bool
    empty()
    {return begin() == end();}

    valueType*
    data()
    {return empty() ? (valueType*)0 : impl.start;}

    const valueType*
    data() const
    {return empty() ? (valueType*)0 : impl.start;}

    referenceType
    at(sizeType n)
    {
        rangeCheck(n);
        return (*this)[n];
    }

    constReferenceType
    at(sizeType n) const
    {
        rangeCheck(n);
        return (*this)[n];
    }

    constReferenceType
    front() const
    {
        if(empty())
            std::cerr << " empty container" << std::endl;
        return *begin();
    }

    referenceType
    front()
    { 
        if(empty())
            std::cerr << " empty container" << std::endl;
        return *begin();
    }

    referenceType
    back()
    {return *(end() - 1);}

    constReferenceType
    back() const
    {return *(end() - 1);}

    //将容器中的元素析构但并未释放内存
    void clear()
    {eraseAtEnd(this->impl.start);}

    void swap(YVector& other)
    {this->impl.swapData(other.impl);}

    void push_back(const valueType& value)
    {
        if(this->impl.finish != this->impl.endOfStorage)
        {
            construct(this->impl.finish,value);
            ++this->impl.finish;
        }
        else
            reallocInsert(end(),value);
    }

    void push_back(valueType&& value)
    {emplace_back(std::move(value));}

    void reserve(sizeType n);

    iterator 
    insert(constIterator position,const valueType& x);

    iterator 
    insert(constIterator position,valueType&& x)
    {return insertRval(position,std::move(x));}

    iterator 
    insert(constIterator positon,sizeType n,const valueType& x)
    {
        differenceType offset = positon - cbegin();
        fillInsert(begin() + offset,n,x);
        return begin() + offset;
    }

    YVector&
    operator=(YVector& other);

    void pop_back() noexcept
    {
        if(size() >= 1)
        {
            --this->impl.finish;
            destroy(this->impl.finish);
        }
    }

    iterator
    erase(constIterator position)
    {return mErase(begin() + (position - cbegin()));}

    iterator
    erase(constIterator first,constIterator last)
    {
        auto beg = begin();
        auto cbeg = cbegin();
        return mErase(beg + (first - cbeg),beg + (last - cbeg));
    }

    template<typename... Args>
    iterator
    emplace(constIterator positon,Args&&... args)
    {return emplaceAux(positon,std::forward<Args>(args)...);}

    template<typename... Args>
    referenceType
    emplace_back(Args&&... args);

    // 下面这个inert函数的版本是不健全的写法
    // 在stl库中使用了std::_RequireInputIter<_InputIterator>来判断作为复制对象的interator是否可转换为IuputIterator类型
    // 比较健全的实现需要实现一些模板函数
    // template<typename InputIterator,
    //     typename = RequireForwardIter<InputIterator>>
    // iterator
    // insert(constIterator position,InputIterator first,InputIterator last)
    // {
    //     differenceType offset = position - cbegin();
    //     insertDispatch(begin() + offset,first,last,false_type());
    //     return begin() + offset;
    // }




    /*
    以下是未实现的函数

    (实现待考虑)
    assign()

    (实现待考虑)
    resize()
    
    (实现待考虑)
    shrink_to_fit()
    
    (实现待考虑)
    insert(iterator,initializer_list<value_type>)

    (实现待考虑)
    operator=(initializer_list<valueType> l)

    (实现待考虑)
    vector&
    operator=(vector&& __x) noexcept(_Alloc_traits::_S_nothrow_move())

    (实现待考虑)
    Yvector(initializer_list<valueType> l) : base() {}

    */

    
protected:
    //rangeCheck函数只用于at()
    void rangeCheck(sizeType n) const
    {
        if(n >= this->size())
            std::cerr << "out of range index" << std::endl;
    }

    //defaul初始化函数
    void defaultInitialize(sizeType n)
    {this->impl.finish = uninitializedDefaultNA(this->impl.start,n);}

    //指定值的初始化
    void fillInitialize(sizeType n,const valueType& value)
    {this->impl.finish = uninitializedFillN(this->impl.start,n,value);}

    //使用一对迭代器进行初始化
    template<typename _InputIterator>
    void rangeInitialize(_InputIterator first,_InputIterator last)
    {
        const sizeType n = YSTL::distance(first,last);
        this->impl.start = this->mAllocator(n);
        this->impl.endOfStorage = this->impl.start + n;
        this->impl.finish = uninitializedCopy(first,last,this->impl.start);
    }

    //initializeDispatch 有两个分支 是两个不同容器构造的不同处理
    template<typename _InputIterator>
    void initializeDispatch(_InputIterator first,_InputIterator last,false_type)
    {rangeInitialize(first,last);}

    template<typename Integer>
    void initializeDispatch(Integer n,Integer value,true_type)
    {
        this->impl.start = mAllocator(n);
        this->impl.endOfStorage = this->impl.start + n;
        this->impl.finish = this->impl.start + n;
        fillInitialize(n,value);
    }

    //使用迭代器的拷贝函数
    template<typename _ForwardIterator>
    pointerType
    allocateAndCopy(sizeType n,_ForwardIterator first,_ForwardIterator last)
    {
        pointerType result = this->mAllocator(n);
        uninitializedCopy(first,last,result);
        return result;
    }

    //在容器中析构指定位置到尾部的所有元素
    void eraseAtEnd(pointerType pos)
    {
        if(sizeType n = this->impl.finish - pos)
        {
            destroy(pos,this->impl.finish);
            this->impl.finish = pos;
        }
    }

    //检查增加的长度是否合法并返回合法的长度
    //当n大于现有size()返回size() + n 否则返回 2 * size()
    sizeType
    checkLen(sizeType n,const char* s)
    {
        if(maxSize() - size() < n)
            std::cerr << s << std::endl;
        const sizeType len = size() + std::max(size(),n);
        return (len < size() || len > maxSize()) ? maxSize() : len;
    }

    template<typename Arg>
    void
    insertAux(iterator positon,Arg&& arg)
    {
        construct(this->impl.finish,std::move(*(this->impl.finish - 1)));
        ++this->impl.finish;
        std::move_backward(positon.base(),
                        this->impl.finish - 2,
                        this->impl.finish - 1);
        *positon = std::forward<Arg>(arg);
    }

    //在指定位置插入元素 执行这个插入时会增加容器的大小
    template<typename... Args>
    void reallocInsert(iterator position,Args&&... args);
    
    //insert插入右值版本的内部函数
    iterator
    insertRval(constIterator position, valueType&& x);

    //往容器指定位置添加指定数量的元素的内部函数
    void fillInsert(iterator position,sizeType n,const valueType& x);

    //移除指定位置元素的内部函数
    iterator 
    mErase(iterator positon)
    {
        if(positon + 1 != end())
            std::move(positon.base() + 1,end().base(),positon.base());
            // std::move_backward((positon + 1).base(),end().base(),end().base() - 1);
        --this->impl.finish;
        destroy(this->impl.finish);
        return positon;
    }

    //移除指定范围内的元素
    iterator
    mErase(iterator first,iterator last)
    {
        if(first != last)
        {
            if(last != end())
                std::move(last.base(),end().base(),first.base());
            eraseAtEnd(first.base() + (end() - last));
        }
        return first;
    }

    //emplace的内部函数
    template<typename... Args>
    iterator
    emplaceAux(constIterator positon,Args&&... args);



    // template<typename _InputIterator>
    // void insertDispatch(iterator position,_InputIterator first,_InputIterator last,false_type)
    // {rangeInsert(position,first,last,iteratorCategory(first));}

    // //在容器指定位置插入first到last的元素
    // template<typename _ForwardIterator>
    // void rangeInsert(iterator position,_ForwardIterator first,_ForwardIterator last
    // ,ForwardIterator);

private:
}; 

}

#include "../src/YSTLVector.cpp"

#endif