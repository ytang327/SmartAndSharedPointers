#ifndef UNIQUEPTR__HPP__
#define UNIQUEPTR__HPP__

#include <cstddef>
#include <stdio.h>

namespace cs540{
    class ObjectDeleterBase{
        public:
            ObjectDeleterBase() = default;
            virtual ~ObjectDeleterBase(){};
    };

    template <typename U>
    class ObjectDeleter : public ObjectDeleterBase{
        public:
            ObjectDeleter() = default;
            ObjectDeleter(U *);
            ~ObjectDeleter();
        private:
            U *object_deleter;
    };
    
    template <typename U>
    ObjectDeleter<U>::ObjectDeleter(U *p):object_deleter(p){}

    template <typename U>
    ObjectDeleter<U>::~ObjectDeleter(){
        delete object_deleter;
    }

    template <typename T>
    class UniquePtr{
        public:
            UniquePtr();
            template <typename U> explicit UniquePtr(U *);
            UniquePtr(UniquePtr &&);
            template <typename U> UniquePtr(UniquePtr<U>&&);
            UniquePtr(const UniquePtr&) = delete;
            UniquePtr& operator=(const UniquePtr&) = delete;
            ~UniquePtr();
            T *get() const;
            template <typename U> void reset(U *);
            void reset(std::nullptr_t);
            UniquePtr& operator=(UniquePtr&&);
            template <typename U> UniquePtr& operator=(UniquePtr<U>&&);
            T& operator*() const;
            T *operator->() const;
            explicit operator bool() const;
            template <typename T1, typename T2>
            friend bool operator==(const UniquePtr<T1>&, const UniquePtr<T2>&);
            template <typename T1>
            friend bool operator==(const UniquePtr<T1>&, std::nullptr_t);
            template <typename T1>
            friend bool operator==(std::nullptr_t, const UniquePtr<T1>&);
            template <typename T1, typename T2>
            friend bool operator!=(const UniquePtr<T1>&, const UniquePtr<T2>&);
            template <typename T1>
            friend bool operator!=(const UniquePtr<T1>&, std::nullptr_t);
            template <typename T1>
            friend bool operator!=(std::nullptr_t, const UniquePtr<T1>&);
            T *ptr;
            ObjectDeleterBase *obj_deleter;
    };

    template <typename T>
    UniquePtr<T>::UniquePtr():ptr(nullptr),obj_deleter(nullptr){}

    template <typename T>
    template <typename U> UniquePtr<T>::UniquePtr(U *p):ptr(p),
        obj_deleter(new typename cs540::ObjectDeleter<U>(p)){}

    template <typename T>
    UniquePtr<T>::UniquePtr(UniquePtr&& p):ptr(p.ptr),
        obj_deleter(p.obj_deleter){
        p.ptr = nullptr;
        p.obj_deleter = nullptr;
    }
    template <typename T>
    template <typename U> UniquePtr<T>::UniquePtr(UniquePtr<U>&& p):ptr(p.ptr),
        obj_deleter(p.obj_deleter){
            p.ptr = nullptr;
            p.obj_deleter = nullptr; 
    }
    template <typename T>
    UniquePtr<T>::~UniquePtr(){
        delete obj_deleter;
    }

    template <typename T>
    T* UniquePtr<T>::get() const{
        return ptr;
    }
    template <typename T>
    template <typename U> void UniquePtr<T>::reset(U *p){
        delete obj_deleter;
        ptr = p;
        obj_deleter = new typename cs540::ObjectDeleter<U>(p);
    }
    template <typename T>
    void UniquePtr<T>::reset(std::nullptr_t p){ 
        delete obj_deleter;
        ptr = p;
        obj_deleter = p;
    }
    template <typename T>
    UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& p){
        if (*this != p){
            delete obj_deleter;
            this->ptr = p.ptr;
            this->obj_deleter = p.obj_deleter;
            p.ptr = nullptr;
            p.obj_deleter = nullptr;
        }
        return *this;
    }

    template <typename T>
    template <typename U> UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<U>&& p){
        if (*this != p){
            delete obj_deleter;
            this->ptr = p.ptr;
            this->obj_deleter = p.obj_deleter;
            p.ptr = nullptr;
            p.obj_deleter = nullptr;
        }
        return *this;

    }
    template <typename T>
    T& UniquePtr<T>::operator*() const{
        return *ptr;
    }
    template <typename T>
    T* UniquePtr<T>::operator->() const{
        return ptr;

    }
    template <typename T>
    UniquePtr<T>::operator bool() const{
        return (ptr != nullptr);
    }

    template <typename T1, typename T2>
    bool operator==(const UniquePtr<T1>& ptr1, const UniquePtr<T2>& ptr2){
        return (ptr1.ptr == ptr2.ptr);   
    }

    template <typename T1>
    bool operator==(const UniquePtr<T1>& ptr1, std::nullptr_t ptr2){
        return (ptr1.ptr == ptr2);
    }

    template <typename T1>
    bool operator==(std::nullptr_t ptr1, const UniquePtr<T1> &ptr2){
        return (ptr1 == ptr2.ptr);
    }

    template <typename T1, typename T2>
    bool operator !=(const UniquePtr<T1>& ptr1, const UniquePtr<T2>& ptr2){
        return (ptr1.ptr != ptr2.ptr);
    }

    template <typename T1>
    bool operator!=(const UniquePtr<T1> &ptr1, std::nullptr_t ptr2){
        return (ptr1.ptr != ptr2);
    }

    template <typename T1>
    bool operator!=(std::nullptr_t ptr1, const UniquePtr<T1> &ptr2){
        return (ptr1 != ptr2.ptr);
    }
}

#endif
