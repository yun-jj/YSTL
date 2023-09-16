#ifndef YSTLALLOCATOR
#define YSTLALLOCATOR

#include<type_traits>
#include<iostream>

#include"YSTLConstruct.h"


//空间配置器
namespace YSTL
{
template<typename T>
struct newAllocator
{
    typedef size_t       sizeType;
    typedef size_t       differenceType;
    typedef T*           pointerType;
    typedef const T*     constPointerType;
    typedef T&           referenceType;
    typedef const T&     constReferenceType;
    typedef T            valueType;

    newAllocator() noexcept {}
    newAllocator(const newAllocator<T>&) {}
    ~newAllocator() noexcept {}

    static inline sizeType 
    maxSize() {return sizeType(-1) / sizeof(T);}
    
    static inline pointerType
    address(referenceType v) {return std::__addressof(v);}

    static inline constPointerType
    address(constReferenceType v) {return std::__addressof(v);}

    static inline pointerType
    allocator(sizeType n)
    {
        if(n > maxSize()) std::cout << "allocator size more than max size" <<std::endl;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    static inline void
    deallocator(T* p) 
    {
        if(p == nullptr) return;
        ::operator delete(p);
    }

    static inline void
    destroyN(T* p,sizeType n)
    {
        while(n--)
        {
            destroy(p);
            ++p;
        }
    }

    template<typename T1>
    struct rebind
    {
        typedef newAllocator<T1> other;
    };
};
} // namespace YSTL


#endif