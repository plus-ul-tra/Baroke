#pragma once
#include <mutex>

//Meyers¡¯ Singleton
template<typename T>
class Singleton {

private:
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator =(const Singleton&) = delete;
	Singleton& operator =(Singleton&&) = delete;

	friend T;

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	//thread-safe
	inline static T& GetInstance()
	{
		static T instance;
		return instance;
	}
};