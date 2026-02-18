#pragma once
#include <cassert>
#include <string>
#include <vector>
#include <optional>
#include <limits>
#include <unordered_map>

template <typename T>
class ResourcePool
{
public:
	ResourcePool() : m_counter(0) {}
	ResourcePool(size_t initial_size) : m_counter(0)
	{
		m_storage.reserve(initial_size);
	}
	// add resource
	// unique: generate random postfix to ensure uniqueness
	int AddUniqueWithGeneratedKey(const std::string& key_prefix, const T& data)
	{
		const std::string key = key_prefix + GenerateKey();
		return Add(key, data, true);
	}
	const std::string GetGeneratedKey(std::string& key_prefix)
	{
		return key_prefix + GenerateKey();
	}
	// unique: error if key exists
	int AddUnique(const std::string& key, const T& data)
	{
		return Add(key, data, true);
	}
	int AddIfNotExists(const std::string& key, const T& data)
	{
		return Add(key, data, false);
	}
	// get id by key, -1 if not found
	int GetId(const std::string& key) const
	{
		auto it = m_ids.find(key);
		if (it == m_ids.end())
		{
			return -1;
		}
		return it->second;
	}
	// get resource from id
	const T& Get(int id) const
	{
		bool valid = CheckIdValid(id);
		assert(valid);
		return m_storage[id].value();
	}
	T& Get(int id)
	{
		bool valid = CheckIdValid(id);
		assert(valid);
		return m_storage[id].value();
	}
	// number of stored resources
	size_t Size() const
	{
		return m_storage.size();
	}
	std::vector<int> GetAllIds() const
	{
		std::vector<int> res;
		for (const auto& pair : m_ids)
		{
			res.push_back(pair.second);
		}
		return res;
	}
	void Remove(int id)
	{
		bool valid = CheckIdValid(id);
		assert(valid);
		// remove from id map
		for (auto it = m_ids.begin(); it != m_ids.end(); ++it)
		{
			if (it->second == id)
			{
				m_ids.erase(it);
				break;
			}
		}
		// remove from storage
		m_storage[id].reset();
		// add to free indices
		m_free_indices.push_back(static_cast<std::size_t>(id));
	}
private:
	// get or get resource id
	int Add(const std::string& key, const T& data, bool force_new = false)
	{

		auto it = m_ids.find(key);
		if (it != m_ids.end())
		{
			if (force_new)
			{
				assert(false);
			}
			return it->second;
		}
		int id{ -1 };
		if (!m_free_indices.empty())
		{
			id = static_cast<int>(m_free_indices.back());
			m_free_indices.pop_back();
			m_storage[id].emplace(data);
		}
		else
		{
			id = static_cast<int>(m_storage.size());
			// assert(id <= MAX_ID);
			m_storage.emplace_back(data);
		}
		m_ids.emplace(key, id);
		return id;
	}
	bool CheckIdValid(int id) const
	{
		return id >= 0 && id < static_cast<int>(m_storage.size()) && m_storage[id].has_value();
	}
	std::string GenerateKey()
	{
		std::string k = "__gen_" + std::to_string(m_counter++);
		return k;
	}
	std::unordered_map<std::string, int> m_ids;
	// in most cases, resources are not removed
	// not using union / variant to store data and index together for simplicity
	std::vector<std::optional<T> > m_storage;
	std::vector<std::size_t> m_free_indices;
	uint64_t m_counter{ 0 }; // random key counter
	// static const int MAX_ID{ std::numeric_limits<int>::max() };
};
