#include <array>
#include <initializer_list>
#include <exception>

template <typename T, size_t N>
class SmallVector
{
public:

    using iterator = typename std::array<T, N>::iterator;
    using const_iterator = typename std::array<T, N>::const_iterator;

    SmallVector(uint8_t n = 0) : _size(n) {
        if (_size > N) {
            throw std::exception("SmallVector overflow");
        }
    }

    SmallVector(const SmallVector& other) = default;
    SmallVector(SmallVector&& other) = default;

    SmallVector(std::initializer_list<T> init)
    {
        _size = init.size();
        for (int i = 0; i < _size; i++) { _storage[i] = init[i]; }
    }

    void push_back(T val) {
        _storage[_size++] = val;
        if (_size > N) {
            throw std::exception("SmallVector overflow");
        }
    }

    void pop_back() {
        if (_size == 0) {
            throw std::exception("SmallVector underflow");
        }
        back().~T(); // call destructor
        _size--;
    }

    size_t size() const { return _size; }

    void clear() { while (_size > 0) { pop_back(); } }

    T& front() { return _storage.front(); }
    const T& front() const { return _storage.front(); }

    T& back() { return _storage[_size - 1]; }
    const T& back() const { return _storage[_size - 1]; }

    iterator begin() { return _storage.begin(); }
    const_iterator begin() const { return _storage.begin(); }

    iterator end() { return _storage.begin() + _size; }
    const_iterator end() const { return _storage.begin() + _size; }

    T& operator[](uint8_t index) { return _storage[index]; }
    const T& operator[](uint8_t index) const { return _storage[index]; }

    T& data() { return _storage.data(); }
    const T& data() const { return _storage.data(); }

private:
    std::array<T, N> _storage;
    uint8_t _size = 0;

};