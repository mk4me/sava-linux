#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <assert.h>
#include <iostream>

template<typename Type>
class Singleton
{
    public:
        static Type& getInstance() 
		{
			assert(ms_Instance != nullptr && "Singleton::Singleton(): ms_Instance is a NULL"); 
			return *ms_Instance;
		}
        static Type* getPointer() 
		{
			return ms_Instance;
		}

    protected:
        Singleton()
        {
            assert(ms_Instance == nullptr && "Singleton::Singleton(): ms_Instance is not a NULL");
            ms_Instance = static_cast<Type*>(this);
        }
        virtual ~Singleton()
        {
            ms_Instance = 0;
        }

    private:
        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);

        static Type* ms_Instance;
};

template<typename Type>
Type* Singleton<Type>::ms_Instance = 0;

#endif // __SINGLETON_H__