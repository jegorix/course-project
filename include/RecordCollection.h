#ifndef RECORDCOLLECTION_H
#define RECORDCOLLECTION_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

/**
 * Шаблонный контейнер с пользовательским итератором.
 * Хранит объекты произвольного типа и предоставляет базовые операции.
 */
template <typename T>
class RecordCollection {
public:
    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(pointer ptr, pointer beginPtr, pointer endPtr)
            : current(ptr), begin(beginPtr), end(endPtr) {}

        reference operator*() const {
            if (current < begin || current >= end) {
                throw std::out_of_range("Итератор указывает на некорректную позицию");
            }
            return *current;
        }

        pointer operator->() const {
            if (current < begin || current >= end) {
                throw std::out_of_range("Итератор указывает на некорректную позицию");
            }
            return current;
        }

        Iterator& operator++() {
            ++current;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            --current;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp(*this);
            --(*this);
            return tmp;
        }

        Iterator& operator+=(difference_type offset) {
            current += offset;
            return *this;
        }

        Iterator& operator-=(difference_type offset) {
            current -= offset;
            return *this;
        }

        Iterator operator+(difference_type offset) const {
            return Iterator(current + offset, begin, end);
        }

        Iterator operator-(difference_type offset) const {
            return Iterator(current - offset, begin, end);
        }

        difference_type operator-(const Iterator& other) const {
            return current - other.current;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        bool operator<(const Iterator& other) const {
            return current < other.current;
        }

        bool operator>(const Iterator& other) const {
            return current > other.current;
        }

        bool operator<=(const Iterator& other) const {
            return current <= other.current;
        }

        bool operator>=(const Iterator& other) const {
            return current >= other.current;
        }

    private:
        pointer current;
        pointer begin;
        pointer end;
    };

    RecordCollection() = default;

    explicit RecordCollection(std::vector<T> values)
        : records(std::move(values)) {}

    Iterator begin() {
        return Iterator(records.data(), records.data(), records.data() + records.size());
    }

    Iterator end() {
        return Iterator(records.data() + records.size(), records.data(), records.data() + records.size());
    }

    typename std::vector<T>::const_iterator begin() const {
        return records.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return records.end();
    }

    std::size_t size() const {
        return records.size();
    }

    bool empty() const {
        return records.empty();
    }

    void reserve(std::size_t capacity) {
        records.reserve(capacity);
    }

    void add(const T& value) {
        records.push_back(value);
    }

    void add(T&& value) {
        records.push_back(std::move(value));
    }

    T& at(std::size_t index) {
        return records.at(index);
    }

    const T& at(std::size_t index) const {
        return records.at(index);
    }

    T& operator[](std::size_t index) {
        return records[index];
    }

    const T& operator[](std::size_t index) const {
        return records[index];
    }

    void clear() {
        records.clear();
    }

    template <typename Predicate>
    Iterator findIf(Predicate predicate) {
        for (auto it = begin(); it != end(); ++it) {
            if (predicate(*it)) {
                return it;
            }
        }
        return end();
    }

    template <typename Predicate>
    void removeIf(Predicate predicate) {
        std::size_t writeIndex = 0;
        for (std::size_t readIndex = 0; readIndex < records.size(); ++readIndex) {
            if (!predicate(records[readIndex])) {
                if (writeIndex != readIndex) {
                    records[writeIndex] = std::move(records[readIndex]);
                }
                ++writeIndex;
            }
        }
        records.resize(writeIndex);
    }

    template <typename Compare>
    void insertionSort(Compare compare) {
        for (std::size_t i = 1; i < records.size(); ++i) {
            T key = records[i];
            std::size_t j = i;
            while (j > 0 && compare(key, records[j - 1])) {
                records[j] = records[j - 1];
                --j;
            }
            records[j] = std::move(key);
        }
    }

    template <typename Function>
    void forEach(Function fn) {
        for (auto& value : records) {
            fn(value);
        }
    }

    std::vector<T>& data() {
        return records;
    }

    const std::vector<T>& data() const {
        return records;
    }

private:
    std::vector<T> records;
};

#endif // RECORDCOLLECTION_H

