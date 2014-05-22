#ifndef SHAREDPTR__HPP__
#define SHAREDPTR__HPP__

#include <cstddef>
#include <iostream>
#include <thread>
#include <mutex>

namespace cs540{
    template <typename T>
    class SharedPtr;

    class ControlBlockBase{
        template <typename T> friend class SharedPtr;
        public:
            ControlBlockBase() = default;
            ControlBlockBase(int);
            virtual ~ControlBlockBase(){};
            int get_ref_count();
        protected:
            int ref_count;
            std::mutex m;
    };

    ControlBlockBase::ControlBlockBase(int ref_count):ref_count(ref_count){}
    int ControlBlockBase::get_ref_count(){ return ref_count;}

    template <typename U>
    class ControlBlock : public ControlBlockBase{
        public:
            ControlBlock() = default;
            ControlBlock(U *, int);
            ~ControlBlock();
        private:
            U *obj_ptr;
    };

    template <typename U>
    ControlBlock<U>::ControlBlock(U *p, int ref_count):
        ControlBlockBase(ref_count),
        obj_ptr(p){}

    template <typename U>
    ControlBlock<U>::~ControlBlock(){
        delete obj_ptr;
    }

    template <typename T>
    class SharedPtr{
        public:
            SharedPtr();
            template<typename U>
            explicit SharedPtr(U *);
            SharedPtr(const SharedPtr&);
            template <typename U>
            SharedPtr(const SharedPtr<U>&);
            SharedPtr(SharedPtr&&);
            template <typename U>
            SharedPtr(SharedPtr<U>&&);
            ~SharedPtr();
            T *get() const;
            template <typename U> void reset(U *);
            void reset(std::nullptr_t);
            SharedPtr& operator=(SharedPtr&&);
            template <typename U> SharedPtr& operator=(SharedPtr<U>&&);
            SharedPtr& operator=(const SharedPtr&);
            template <typename U> SharedPtr& operator=(const SharedPtr<U>&);
            T& operator*() const;
            T *operator->() const;
            explicit operator bool() const;
            void decrement();
            template <typename U>void increment(const SharedPtr<U>&);
            void increment(const SharedPtr&);
            T *ptr;
            ControlBlockBase *ctrl_blk_ptr;
    };

    template <typename T>
    SharedPtr<T>::SharedPtr():ptr(nullptr),ctrl_blk_ptr(nullptr){}

    template <typename T>
    template <typename U> SharedPtr<T>::SharedPtr(U *p):ptr(p),
        ctrl_blk_ptr(new typename cs540::ControlBlock<U>(p, 1)){}

    template <typename T>
    SharedPtr<T>::SharedPtr(const SharedPtr& p):ptr(p.ptr),
        ctrl_blk_ptr(p.ctrl_blk_ptr){
        increment(p);
    }


    template <typename T>
    template <typename U> SharedPtr<T>::SharedPtr(const SharedPtr<U>& p):
           ptr(p.ptr), ctrl_blk_ptr(p.ctrl_blk_ptr){
           increment(p);
    }

    template <typename T>
    SharedPtr<T>::SharedPtr(SharedPtr&& p):ptr(p.ptr),
        ctrl_blk_ptr(p.ctrl_blk_ptr){
            p.ptr = nullptr;
            p.ctrl_blk_ptr = nullptr;
    }

    template <typename T>
    template <typename U> SharedPtr<T>::SharedPtr(SharedPtr<U>&& p):ptr(p.ptr),
        ctrl_blk_ptr(p.ctrl_blk_ptr){
            p.ptr = nullptr;
            p.ctrl_block_ptr = nullptr;
    }

    template <typename T>
    void SharedPtr<T>::decrement(){
        if (ctrl_blk_ptr != nullptr){
            ctrl_blk_ptr->m.lock();
            --(ctrl_blk_ptr->ref_count);
            bool is_zero = (ctrl_blk_ptr->ref_count == 0);
            ctrl_blk_ptr->m.unlock();
            if (is_zero){
                delete ctrl_blk_ptr; 
            }
        }
    }
    template <typename T>
    template <typename U> void SharedPtr<T>::increment(const SharedPtr<U>& p){
        if (p.ctrl_blk_ptr != nullptr){
            ctrl_blk_ptr->m.lock();
            ++(ctrl_blk_ptr->ref_count);
            ctrl_blk_ptr->m.unlock();
        }
    }

    template <typename T>
    void SharedPtr<T>::increment(const SharedPtr& p){
        if (p.ctrl_blk_ptr != nullptr){
            ctrl_blk_ptr->m.lock();
            ++(ctrl_blk_ptr->ref_count);
            ctrl_blk_ptr->m.unlock();
        }
    }
    template <typename T>
    SharedPtr<T>::~SharedPtr(){
        decrement();
    }

    template <typename T>
    T* SharedPtr<T>::get() const{
        return ptr;
    }

    template <typename T>
    template <typename U> void SharedPtr<T>::reset(U *p){
        decrement();
        ptr = p;
        ctrl_blk_ptr = new typename cs540::ControlBlock<U>(p, 1);
    }
    
    template <typename T>
    void SharedPtr<T>::reset(std::nullptr_t p){
        decrement();
        ptr = p;
        ctrl_blk_ptr = p;
    }

    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& p){
        if (this != &p){
            decrement();
            this->ptr = p.ptr;
            this->ctrl_blk_ptr = p.ctrl_blk_ptr;
            p.ptr = nullptr;
            p.ctrl_blk_ptr = nullptr;
        }
        return *this;
    }

    template <typename T>
    template <typename U> SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& p){
        decrement();
        this->ptr = p.ptr;
        this->ctrl_blk_ptr = p.ctrl_blk_ptr;
        p.ptr = nullptr;
        p.ctrl_blk_ptr = nullptr;
        return *this;
    }

    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& p){
        if (this != &p){
            decrement();
            this->ptr = p.ptr;
            this->ctrl_blk_ptr = p.ctrl_blk_ptr;
            increment(p);
        }
        return *this;

    }

    template <typename T>
    template <typename U> SharedPtr<T>& SharedPtr<T>::operator=(
            const SharedPtr<U>& p){
            decrement();
            this->ptr = p.ptr;
            this->ctrl_blk_ptr = p.ctrl_blk_ptr;
            increment(p);
            return *this;
    }

    template <typename T>
    T& SharedPtr<T>::operator*() const{
        return *ptr;
    }

    template <typename T>
    T* SharedPtr<T>::operator->() const{
        return ptr;
    }

    template <typename T>
    SharedPtr<T>::operator bool() const{
        return (ptr != nullptr);
    }

    template <typename T1, typename T2>
    bool operator==(const SharedPtr<T1>& ptr1, const SharedPtr<T2>& ptr2){
        return (ptr1.ptr == ptr2.ptr);
    }

    template <typename T1>
    bool operator==(const SharedPtr<T1>& ptr1, std::nullptr_t ptr2){
        return (ptr1.ptr == ptr2);
    }

    template <typename T1>
    bool operator==(std::nullptr_t ptr1, const SharedPtr<T1> &ptr2){
        return (ptr1 == ptr2.ptr);
    }

    template <typename T1, typename T2>
    bool operator !=(const SharedPtr<T1>& ptr1, const SharedPtr<T2>& ptr2){
        return (ptr1.ptr != ptr2.ptr);
    }

    template <typename T1>
    bool operator!=(const SharedPtr<T1> &ptr1, std::nullptr_t ptr2){
        return (ptr1.ptr != ptr2);
    }

    template <typename T1>
    bool operator!=(std::nullptr_t ptr1, const SharedPtr<T1> &ptr2){
        return (ptr1 != ptr2.ptr);
    }
}

#endif
