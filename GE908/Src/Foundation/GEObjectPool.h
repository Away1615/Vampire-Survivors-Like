#pragma once

#include <type_traits>

class GEPoolable {
public:
    virtual ~GEPoolable() = default;
    virtual bool isActiveElement() const = 0;
};

// Owns and reuses pooled objects.
template <typename T>
class GEObjectPool {
private:
    static_assert(std::is_pointer_v<T>, "GEObjectPool owns pointer elements.");
    static_assert(std::is_base_of_v<GEPoolable, std::remove_pointer_t<T>>,
        "GEObjectPool elements must implement GEPoolable.");

    T* _data = nullptr;
    unsigned int _size = 0;
    unsigned int _capacity = 0;

    void reserveSlots(unsigned int newCapacity) {
        if (newCapacity <= _capacity) return;

        T* newData = new T[newCapacity]{};
        for (unsigned int i = 0; i < _size; ++i)
            newData[i] = _data[i];
        delete[] _data;

        _data = newData;
        _capacity = newCapacity;
    }

    void destroyElements() {
        for (unsigned int i = 0; i < _size; ++i) {
            delete _data[i];
            _data[i] = nullptr;
        }
    }

    void release() {
        destroyElements();
        delete[] _data;
        _data = nullptr;
        _size = 0;
        _capacity = 0;
    }

public:
    GEObjectPool() = default;

    explicit GEObjectPool(unsigned int slotCount) {
        reset(slotCount);
    }

    ~GEObjectPool() {
        release();
    }

    GEObjectPool(const GEObjectPool&) = delete;
    GEObjectPool& operator=(const GEObjectPool&) = delete;

    GEObjectPool(GEObjectPool&& other) noexcept
        : _data(other._data), _size(other._size), _capacity(other._capacity) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    GEObjectPool& operator=(GEObjectPool&& other) noexcept {
        if (this != &other) {
            release();
            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;
            other._data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        return *this;
    }

    unsigned int size() const { return _size; }

    unsigned int countActive() const {
        unsigned int count = 0;
        for (unsigned int i = 0; i < _size; ++i)
            if (_data[i] && _data[i]->isActiveElement()) ++count;
        return count;
    }

    T getAt(unsigned int index) const {
        return index < _size ? _data[index] : nullptr;
    }

    T findInactive() const {
        for (unsigned int i = 0; i < _size; ++i) {
            if (_data[i] && !_data[i]->isActiveElement()) return _data[i];
        }
        return nullptr;
    }

    // The pool takes ownership.
    void add(T value) {
        for (unsigned int i = 0; i < _size; ++i) {
            if (!_data[i]) {
                _data[i] = value;
                return;
            }
        }

        if (_size >= _capacity)
            reserveSlots(_capacity == 0 ? 4 : _capacity * 2);
        _data[_size++] = value;
    }

    void reset(unsigned int slotCount) {
        destroyElements();
        reserveSlots(slotCount);
        for (unsigned int i = 0; i < slotCount; ++i)
            _data[i] = nullptr;
        _size = slotCount;
    }
};
