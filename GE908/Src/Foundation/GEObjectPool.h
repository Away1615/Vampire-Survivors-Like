#pragma once
#include <utility>

class GEPoolable {
public:
    virtual ~GEPoolable() = default;
    virtual bool isActiveElement() const = 0;
};


template <typename T>
class GEObjectPool {
private:
    // The pool owns its pointer elements. Inactive objects stay allocated for reuse.
    mutable T* _data = nullptr;    // data

    unsigned int _size = 0;        // number of slots currently in use
    unsigned int _capacity = 0;    // nallocated array length

public:
    GEObjectPool() = default;

    explicit GEObjectPool(unsigned int capacity) { reserve(capacity); }

    ~GEObjectPool() { clear(); }

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
            clear();
            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;
            other._data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        return *this;
    }

    // ======== Basic Function ========

    unsigned int size() const { return _size; }

    unsigned int capacity() const { return _capacity; }

    unsigned int countActive() const {
        unsigned int count = 0;
        for (unsigned int i = 0; i < _size; ++i) {
            if (!_data[i] || !_data[i]->isActiveElement()) continue;
            if (i != count) std::swap(_data[count], _data[i]);
            ++count;
        }
        return count;
    }

    bool empty() const { return countActive() == 0; }

    T& operator[](unsigned int i) { return _data[i]; }
    const T& operator[](unsigned int i) const { return _data[i]; }

    T* data() { return _data; }
    const T* data() const { return _data; }

    T findInactive() const {
        for (unsigned int i = 0; i < _size; ++i) {
            if (_data[i] && !_data[i]->isActiveElement()) return _data[i];
        }
        return nullptr;
    }

    // ======== Memory management ========

    void reserve(unsigned int new_capacity) {
        if (new_capacity <= _capacity) return;

        T* new_data = new T[new_capacity];
        for (unsigned int i = 0; i < _size; ++i)
            new_data[i] = _data[i];
        delete[] _data;

        _data = new_data;
        _capacity = new_capacity;
    }

    void resize(unsigned int new_size) {
        if (new_size > _capacity)
            reserve(new_size);
        // fill new slots with nullptr
        for (unsigned int i = _size; i < new_size; ++i)
            _data[i] = nullptr;
        _size = new_size;
    }

    // ======== Core API ========

    void add(const T& value) {
        // reuse empty slot if available
        for (unsigned int i = 0; i < _size; ++i)
            if (_data[i] == nullptr) {
                _data[i] = value;
                return;
            }

        // otherwise append
        if (_size >= _capacity)
            reserve((_capacity == 0) ? 4 : _capacity * 2);

        _data[_size++] = value;
    }

    void remove(const T& value) {
        for (unsigned int i = 0; i < _size; ++i)
            if (_data[i] == value) {
                _data[i] = nullptr;
                return;
            }
    }

    void clear() {
        destroyAll();
        delete[] _data;
        _data = nullptr;
        _size = 0;
        _capacity = 0;
    }

    void destroyAll() {
        for (unsigned int i = 0; i < _size; ++i) {
            if (_data[i]) {
                delete _data[i];
                _data[i] = nullptr;
            }
        }
    }

    template <typename Func>
    void forEachActive(Func&& func) const {
        for (unsigned int i = 0; i < _size; ++i)
            if (_data[i] && _data[i]->isActiveElement())
                func(_data[i], i);
    }

    void fillNull(unsigned int count) {
        resize(count);
        for (unsigned int i = 0; i < count; ++i)
            _data[i] = nullptr;
    }
};
