#pragma once

template<class T>
class Singleton
{
protected:
	Singleton() {}
	~Singleton() {}

public:
	static T* GetSingleton()
	{
		if (!m_Instance)
		{
			m_Instance = new T;
			return m_Instance;
		}

		return m_Instance;
	}

public:
	static T* m_Instance;

};