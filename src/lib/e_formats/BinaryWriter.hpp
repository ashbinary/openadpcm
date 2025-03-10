#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <bit>
#include <concepts>
#include <span>

template<typename T>
concept Number = std::is_arithmetic_v<T>;

class BinaryWriter {
    static constexpr bool s_ByteSwap = false;

    uintptr_t m_Data;
    size_t m_Size;
    size_t m_Position = 0;

    bool IsValidPosition(size_t pos) const {
        return 0 <= pos && pos < m_Size;
    }

    void AssertPositionValid(size_t pos) const {
        /* TODO */
    }


    template<Number T>
    static T ByteSwap(T value) {
        if constexpr(s_ByteSwap) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    public:

    BinaryWriter(void* data, size_t pos) : m_Data(reinterpret_cast<uintptr_t>(data)), m_Size(pos) {}

    void Seek(size_t pos) {
        AssertPositionValid(pos);
        m_Position = pos;
    }

    template<Number T>
    void Write(T value) {
        AssertPositionValid(m_Position + sizeof(T));
        *reinterpret_cast<T*>(m_Data + m_Position) = ByteSwap(value);
        m_Position += sizeof(T);
    }

    template<typename T, size_t Size>
    void Write(std::span<T, Size> data) {
        size_t length = data.size_bytes();
        AssertPositionValid(m_Position + length);
        T* it = reinterpret_cast<T*>(m_Data + m_Position);
        for(size_t i = 0; i < data.size(); i++) {
            Write<T>(data[i]);
        }
    }

    void Write(std::string_view str) {
        size_t length = std::strlen(str.data());
        AssertPositionValid(m_Position + length);
        std::memcpy(reinterpret_cast<void*>(m_Data + m_Position), str.data(), length-1);
    }

    size_t Position() const {
        return m_Position;
    }
};

// int main() {
//     struct {
//         int f1;
//         float f2;
//     } data {};

//     BinaryWriter w(reinterpret_cast<void*>(&data), sizeof(data));
//     w.Write<int>(5);
//     w.Write<float>(1);

//     printf("%x\n", data.f1);
//     printf("%f\n", data.f2);


//     return 0;
// }