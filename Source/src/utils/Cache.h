#pragma once
#include <unordered_map>

namespace Hachiko
{
    template<typename K, typename V>
    class Cache
    {
    public:
        Cache(V);
        ~Cache() = default;

        void Set(const K&, V);
        [[nodiscard]] const V& Get(const K&) const;

        [[nodiscard]] const V& NotFound() const
        {
            return not_found_value;
        }

        void Clear()
        {
            cache.clear();
        }

    private:
        V not_found_value;
        mutable std::unordered_map<K, V> cache{};
    };

    template<typename K, typename V>
    Cache<K, V>::Cache(V not_found_value):
        not_found_value(not_found_value)
    {
    }

    template<typename K, typename V>
    const V& Cache<K, V>::Get(const K& key) const
    {
        // ReSharper disable CppUseAssociativeContains
        if (cache.find(key) != cache.end())
        // ReSharper restore CppUseAssociativeContains
        {
            return cache[key];
        }
        return not_found_value;
    }


    template<typename K, typename V>
    void Cache<K, V>::Set(const K& key, V value)
    {
        cache[key] = value;
    }
}
