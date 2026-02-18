#pragma once
#include <cassert>

template <class T, int CAPACITY>
class List
{
public:
	void Initialize()
	{
		m_count = 0;
		for (int i = 0; i < CAPACITY; i++)
		{
			m_data[i] = nullptr;
		}
	}
	void Finalize()
	{
		m_count = 0;
	}
	void Add(T* element)
	{
		assert(m_count <= CAPACITY - 1);
		m_data[m_count] = element;
		m_count++;
	}
	void Remove(int index)
	{
		// ! will not preserve order
		assert(index < m_count && index >= 0);
		// move last element to current index
		// decrease count by 1
		m_data[index] = m_data[m_count - 1];
		m_data[m_count - 1] = nullptr;
		--m_count;
	}
	void RemoveOrderedPend(int index)
	{
		// preserve order
		// some pointer will be nullptr until calling RemoveOrderedApply
		assert(index < m_count && index >= 0);
		m_data[index] = nullptr;
	}
	void RemoveOrderedApply()
	{
		// remove nullptr
		int tail{ 0 }, previous_count{ m_count };
		for (int i = 0; i < previous_count; i++)
		{
			if (!m_data[i])
			{
				--m_count;
				continue;
			}
			m_data[tail++] = m_data[i];
		}
		for (int i = m_count; i < previous_count; i++)
		{
			m_data[i] = nullptr;
		}
	}
	void Clear()
	{
		for (int i = 0; i < m_count; i++)
		{
			m_data[i] = nullptr;
		}
		m_count = 0;
	}
	void ClearDelete()
	{
		for (int i = 0; i < m_count; i++)
		{
			delete m_data[i];
			m_data[i] = nullptr;
		}
		m_count = 0;
	}
	~List()
	{
		Finalize();
	}
	int GetCount() const
	{
		return m_count;
	}
	T* operator[](size_t i)
	{
		if (i >= m_count)
		{
			return nullptr;
		}
		return m_data[i];
	}
	const T* operator[](size_t i) const
	{
		if (i >= m_count)
		{
			return nullptr;
		}
		return m_data[i];
	}
	bool Empty() const
	{
		return m_count == 0;
	}
	bool Full() const
	{
		return m_count >= CAPACITY - 1;
	}
private:
	T* m_data[CAPACITY]{};
	int m_count{ 0 };
};