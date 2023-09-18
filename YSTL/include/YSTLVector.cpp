#ifndef YSTLVECTORCPP
#define YSTLVECTORCPP


namespace YSTL
{
template<typename T,typename Alloc>
void
YVector<T,Alloc>::
reserve(sizeType n)
{
    if(n > maxSize())
        std::cerr << "error in reserve" << std::endl;
    if(capacity() < n)
    {
        const sizeType oldSize = size();
        pointerType tmp = allocateAndCopy(n,this->impl.start,this->impl.finish);
        YSTL::destroy(this->impl.start,this->impl.finish);
        mDeallocator(this->impl.start,this->impl.endOfStorage - this->impl.start);
        this->impl.start = tmp;
        this->impl.finish = tmp + oldSize;
        this->impl.endOfStorage = this->impl.start + n;
    }
}

template<typename T,typename Alloc>
template<typename... Args>
void 
YVector<T,Alloc>::reallocInsert(iterator position,Args&&... args)
{
    const sizeType len = checkLen(sizeType(1),"reallocInsert");
    pointerType oldStart = this->impl.start;
    pointerType oldFinish = this->impl.finish;
    const sizeType elemsBefore = position - begin();
    pointerType newStart(this->mAllocator(len));
    pointerType newFinish(newStart);

    construct(newStart + elemsBefore,std::forward<Args>(args)...);

    newFinish = pointerType();

    newFinish = uninitializedCopy(oldStart,position.base(),newStart);

    ++newFinish;

    newFinish = uninitializedCopy(position.base(),oldFinish,newFinish);

    destroy(oldStart,oldFinish);
    mDeallocator(oldStart,this->impl.endOfStorage - oldStart);
    this->impl.start = newStart;
    this->impl.finish = newFinish;
    this->impl.endOfStorage = newStart + len;

}

template<typename T,typename Alloc>
typename YVector<T,Alloc>::iterator
YVector<T,Alloc>::insert(constIterator position,const valueType& x)
{
    const sizeType n = position - begin();
    if(this->impl.finish != this->impl.endOfStorage)
    {
        if(position == end())
        {
            construct(this->impl.finish,x);
            ++this->impl.finish;
        }
        else
        {
            insertAux(position,x);
        }
    }
    else
    {
        reallocInsert(begin() + (position - cbegin()),x);
    }

    return iterator(this->impl.start + n);
}

template<typename T,typename Alloc>
typename YVector<T,Alloc>::iterator
YVector<T,Alloc>::insertRval(constIterator position,valueType&& x)
{
    const sizeType n = position - begin();
    if(this->impl.finish != this->impl.endOfStorage)
    {
        if(position == end())
        {
            construct(this->impl.finish,std::move(x));
            ++this->impl.finish;
        }
        else
        {
            insertAux(begin() + n,std::move(x));
        }
    }
    else
    {
        reallocInsert(begin() + (position - cbegin()),x);
    }

    return iterator(this->impl.start + n);
}

template<typename T,typename Alloc>
void
YVector<T,Alloc>::fillInsert(iterator position,sizeType n,const valueType& x)
{
    if(n != 0)
    {
        if(sizeType(this->impl.endOfStorage - this->impl.finish) >= n)
        {
            valueType copyX = x;
            const sizeType elemsAfter = end() - position;
            pointerType oldFinish(this->impl.finish);

            if(elemsAfter > n)
            {
                uninitializedCopy(this->impl.finish - n,this->impl.finish,this->impl.finish);
                this->impl.finish += n;
                std::move_backward(position.base(),oldFinish - n,oldFinish);
                std::fill(position.base(),position.base() + n,copyX);
            }
            else
            {
                this->impl.finish = uninitializedFillN(this->impl.finish,n - elemsAfter,copyX);
                uninitializedCopy(position.base(),oldFinish,this->impl.finish);
                this->impl.finish += elemsAfter;
                std::fill(position.base(),oldFinish,copyX);
            }
        }
        else
        {
            const sizeType len = checkLen(n,"YVector::fillInsert");
            const sizeType elemsBefore = position - begin();
            pointerType newStart(this->mAllocator(len));
            pointerType newFinish(newStart);
            uninitializedFillN(newStart + elemsBefore,n,x);
            newFinish = pointerType();

            newFinish = uninitializedCopy(this->impl.start,position.base(),newStart);
            newFinish += n;
            newFinish = uninitializedCopy(position.base(),this->impl.finish,newFinish);

            destroy(this->impl.start,this->impl.finish);
            mDeallocator(this->impl.start,this->impl.endOfStorage - this->impl.start);

            this->impl.start = newStart;
            this->impl.finish = newFinish;
            this->impl.endOfStorage = newFinish + len;

        }
    }
}

template<typename T,typename Alloc>
template<typename _ForwardIterator>
void
YVector<T,Alloc>::rangeInsert(iterator position,_ForwardIterator first,_ForwardIterator last
    ,ForwardIterator)
{
    if(first != last)
    {
        const sizeType n = distance(first,last);
        if(sizeType(this->impl.endOfStorage - this->impl.start) >= n)
        {
            const sizeType elemsAfter = end() - position;
            pointerType oldFinish(this->impl.finish);
            if(elemsAfter > n)
            {
                uninitializedCopy(this->impl.finish - n,this->impl.finish,this->impl.finish);
                this->impl.finish += n;
                std::move_backward(position.base(),oldFinish - n,oldFinish);
                std::copy(first,last,position);
            }
            else
            {
                _ForwardIterator mid = first;
                advance(mid,elemsAfter);
                uninitializedCopy(mid.base(),last.base(),this->impl.finish);
                this->impl.finish += n - elemsAfter;
                //下面的代码需要move_iterator 暂时搁置 实现代码在vector.tcc 708行
            }
        }
        else
        {

        }
    }
}

};
#endif