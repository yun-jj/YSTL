#ifndef YSTLALLOCATORTRAITS
#define YSTLALLOCATORTRAITS

namespace YSTL 
{
template<typename Alloc>    
struct AllocatorTraits
{
    template<typename T>
      using pointerType = typename T::pointerType;
    template<typename T>
      using constPointer = typename T::constPointer;
    template<typename T>
      using propagateOnContainerMoveAssignment = typename T::propagateOnContainerMoveAssignment;
    template<typename T>
      using alwaysEqual = typename T::alwaysEqual;
    typedef Alloc               allocatorType;
    typedef Alloc::valueType    valueType;
    
};
};

#endif