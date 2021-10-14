#pragma once

// esp-idf includes
#include <mdns.h>

// 3rdparty lib includes
#include <arrayview.h>

class AsyncMdnsResults
{
    template<typename T>
    struct linked_list_iter
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        constexpr linked_list_iter(pointer ptr) noexcept : m_ptr{ptr} {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        linked_list_iter &operator++() { m_ptr = m_ptr->next; return *this; }

        friend bool operator== (const linked_list_iter& a, const linked_list_iter& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const linked_list_iter& a, const linked_list_iter& b) { return a.m_ptr != b.m_ptr; };

    private:
        pointer m_ptr;
    };

public:
    using value_type =             mdns_result_t;
    using pointer =                value_type*;
    using const_pointer =          const value_type*;
    using reference =              value_type&;
    using const_reference =        const value_type&;
    using iterator =               linked_list_iter<value_type>;
    using const_iterator =         linked_list_iter<const value_type>;
    using size_type =              std::size_t;
    using difference_type =        std::ptrdiff_t;
    // not providing reverse_iterator
    // not providing const_reverse_iterator

    AsyncMdnsResults() noexcept = default;
    AsyncMdnsResults(mdns_result_t *results, std::size_t num_results) noexcept :
        m_results{results}, m_num_results{num_results}
    {}

    AsyncMdnsResults(const AsyncMdnsResults &) noexcept = delete;
    AsyncMdnsResults(AsyncMdnsResults &&other) noexcept :
        m_results{std::move(other.m_results)}, m_num_results{std::move(other.m_num_results)}
    {
        other.m_results = {};
        other.m_num_results = {};
    }

    AsyncMdnsResults &operator=(const AsyncMdnsResults &) noexcept = delete;
    AsyncMdnsResults &operator=(AsyncMdnsResults &&other)
    {
        if (&other != this)
        {
            free();
            m_results = std::move(other.m_results);
            other.m_results = {};
            m_num_results = std::move(other.m_num_results);
            other.m_num_results = {};
        }
        return *this;
    }

    ~AsyncMdnsResults()
    {
        free();
    }


    // Iterators.
    constexpr iterator begin() noexcept { return iterator(data()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
    constexpr iterator end() noexcept { return iterator(nullptr); }
    constexpr const_iterator end() const noexcept { return const_iterator(nullptr); }
    // not providing rbegin()
    // not providing rend()
    constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
    constexpr const_iterator cend() const noexcept { return const_iterator(nullptr); }
    // not providing crbegin();
    // not providing crend();

    // Capacity.
    constexpr size_type size() const noexcept { return m_num_results; }
    constexpr size_type max_size() const noexcept { return size(); }
    constexpr bool empty() const noexcept { return data() == nullptr; }

    // Element access.
    // not providing operator[]
    // not providing at()
    // not providing at()
    constexpr reference front() noexcept { return *data(); }
    constexpr const_reference front() const noexcept { return *data(); }
    // not providing back()
    constexpr pointer data() noexcept { return m_results; }
    constexpr const_pointer data() const noexcept { return m_results; }

private:
    void free()
    {
        if (m_results)
        {
            mdns_query_results_free(m_results);
            m_results = {};
        }

        m_num_results = {};
    }

    mdns_result_t *m_results{};
    std::size_t m_num_results{};
};
