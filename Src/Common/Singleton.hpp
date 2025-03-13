#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <type_traits>

// 作用：
// 1、去掉T類的拷貝構造函數、賦值構造函數
// 2、使用默認構造函數
// 3、T類對Singleton友元
// PS:使用友元是爲了調用Singleton的私有析構
#define SINGLETON_OBJECT(T)             \
friend class Singleton<T>;              \
public:                                 \
    T(const T&) = delete;               \
    T& operator=(const T&) = delete;    \
private:                                \
    T()=default;


// 允許子類自定義默認構造函數
#define SINGLETON_OBJECT_NO_DEFCONS(T)  \
friend class Singleton<T>;              \
public:                                 \
    T(const T&) = delete;               \
    T& operator=(const T&) = delete;

template<class T>
class Singleton {

public:
    static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value){
        static T instance;
        return instance;
    }

    virtual ~Singleton() noexcept{}

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() {}

};

#endif
