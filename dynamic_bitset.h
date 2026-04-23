
#ifndef DYNAMIC_BITSET_H
#define DYNAMIC_BITSET_H

#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <string>

namespace sjtu {

struct dynamic_bitset {
    std::vector<uint64_t> bits;
    std::size_t n_bits = 0;

    // 默认构造函数，默认长度为 0
    dynamic_bitset() = default;

    // 除非手动管理内存，否则 = default 即可
    ~dynamic_bitset() = default;

    /**
     * @brief 拷贝构造函数
     * 如果你用 std::vector 来实现，那么这个函数可以直接 = default
     * 如果你手动管理内存，则你可能需要自己实现这个函数
     */
    dynamic_bitset(const dynamic_bitset &) = default;

    /**
     * @brief 拷贝赋值运算符
     * 如果你用 std::vector 来实现，那么这个函数可以直接 = default
     * 如果你手动管理内存，则你可能需要自己实现这个函数
     */
    dynamic_bitset &operator = (const dynamic_bitset &) = default;

    // 初始化 bitset 的大小为 n ，且全为 0.
    dynamic_bitset(std::size_t n) : n_bits(n) {
        bits.assign((n + 63) / 64, 0);
    }

    /**
     * @brief 从一个字符串初始化 bitset。
     * 保证字符串合法，且最低位在最前面。
     * 例如 a =  "0010"，则有:
     * a 的第 0 位是 0
     * a 的第 1 位是 0
     * a 的第 2 位是 1
     * a 的第 3 位是 0
     */
    dynamic_bitset(const std::string &str) : n_bits(str.size()) {
        bits.assign((n_bits + 63) / 64, 0);
        for (std::size_t i = 0; i < n_bits; ++i) {
            if (str[i] == '1') {
                bits[i / 64] |= (uint64_t(1) << (i % 64));
            }
        }
    }

    // 访问第 n 个位的值，和 vector 一样是 0-base
    bool operator [] (std::size_t n) const {
        if (n >= n_bits) return false;
        return (bits[n / 64] >> (n % 64)) & 1;
    }

    // 把第 n 位设置为指定值 val
    dynamic_bitset &set(std::size_t n, bool val = true) {
        if (n >= n_bits) return *this;
        if (val) {
            bits[n / 64] |= (uint64_t(1) << (n % 64));
        } else {
            bits[n / 64] &= ~(uint64_t(1) << (n % 64));
        }
        return *this;
    }

    // 在尾部插入一个位，并且长度加一
    /* 
    补充说明：这里指的是高位，
    比如0010后面push_back(1)应该变为00101
    */
    dynamic_bitset &push_back(bool val) {
        if (n_bits % 64 == 0) {
            bits.push_back(0);
        }
        if (val) {
            bits[n_bits / 64] |= (uint64_t(1) << (n_bits % 64));
        }
        n_bits++;
        return *this;
    }

    // 如果不存在 1 ，则返回 true。否则返回 false
    bool none() const {
        if (n_bits == 0) return true;
        for (std::size_t i = 0; i < bits.size() - 1; ++i) {
            if (bits[i] != 0) return false;
        }
        uint64_t last_mask = get_last_mask();
        if ((bits.back() & last_mask) != 0) return false;
        return true;
    }

    // 如果不存在 0 ，则返回 true。否则返回 false
    bool all() const {
        if (n_bits == 0) return true;
        for (std::size_t i = 0; i < bits.size() - 1; ++i) {
            if (bits[i] != ~uint64_t(0)) return false;
        }
        uint64_t last_mask = get_last_mask();
        if ((bits.back() & last_mask) != last_mask) return false;
        return true;
    }

    // 返回自身的长度
    std::size_t size() const {
        return n_bits;
    }

    /**
     * 所有位运算操作均按照以下规则进行:
     * 取两者中较短的长度那个作为操作长度。
     * 换句话说，我们仅操作两者中重叠的部分，其他部分不变。
     * 在操作前后，bitset 的长度不应该发生改变。
     */

    uint64_t get_mask(std::size_t m) const {
        if (m == 0) return ~uint64_t(0);
        return (uint64_t(1) << m) - 1;
    }

    uint64_t get_last_mask() const {
        if (n_bits == 0) return 0;
        std::size_t m = n_bits % 64;
        if (m == 0) return ~uint64_t(0);
        return (uint64_t(1) << m) - 1;
    }

    // 或操作，返回自身的引用。     a |= b 即 a = a | b
    dynamic_bitset &operator |= (const dynamic_bitset &other) {
        std::size_t common_bits = std::min(n_bits, other.n_bits);
        std::size_t common_words = (common_bits + 63) / 64;
        for (std::size_t i = 0; i < common_words; ++i) {
            uint64_t mask = (i == common_words - 1) ? get_mask(common_bits % 64) : ~uint64_t(0);
            if (mask == 0 && common_bits % 64 == 0 && common_bits > 0) mask = ~uint64_t(0);
            bits[i] |= (other.bits[i] & mask);
        }
        return *this;
    }

    // 与操作，返回自身的引用。     a &= b 即 a = a & b
    dynamic_bitset &operator &= (const dynamic_bitset &other) {
        std::size_t common_bits = std::min(n_bits, other.n_bits);
        std::size_t common_words = (common_bits + 63) / 64;
        for (std::size_t i = 0; i < common_words; ++i) {
            uint64_t mask = (i == common_words - 1) ? get_mask(common_bits % 64) : ~uint64_t(0);
            if (mask == 0 && common_bits % 64 == 0 && common_bits > 0) mask = ~uint64_t(0);
            bits[i] &= (other.bits[i] | ~mask);
        }
        return *this;
    }

    // 异或操作，返回自身的引用。   a ^= b 即 a = a ^ b
    dynamic_bitset &operator ^= (const dynamic_bitset &other) {
        std::size_t common_bits = std::min(n_bits, other.n_bits);
        std::size_t common_words = (common_bits + 63) / 64;
        for (std::size_t i = 0; i < common_words; ++i) {
            uint64_t mask = (i == common_words - 1) ? get_mask(common_bits % 64) : ~uint64_t(0);
            if (mask == 0 && common_bits % 64 == 0 && common_bits > 0) mask = ~uint64_t(0);
            bits[i] ^= (other.bits[i] & mask);
        }
        return *this;
    }

    /**
     * @brief 左移 n 位 。类似无符号整数的左移，最低位会补 0.
     * 例如 a = "1110"
     * a <<= 3 之后，a 应该变成 "0001110"
     * @return 返回自身的引用
     */
    dynamic_bitset &operator <<= (std::size_t n) {
        if (n == 0) return *this;
        std::size_t old_n = n_bits;
        n_bits += n;
        std::vector<uint64_t> new_bits((n_bits + 63) / 64, 0);
        
        std::size_t word_shift = n / 64;
        std::size_t bit_shift = n % 64;
        
        for (std::size_t i = 0; i < bits.size(); ++i) {
            if (bits[i] == 0) continue;
            
            std::size_t target_word = i + word_shift;
            if (target_word < new_bits.size()) {
                new_bits[target_word] |= (bits[i] << bit_shift);
            }
            if (bit_shift > 0 && target_word + 1 < new_bits.size()) {
                new_bits[target_word + 1] |= (bits[i] >> (64 - bit_shift));
            }
        }
        
        bits = std::move(new_bits);
        clean_unused_bits();
        return *this;
    }

    /**
     * @brief 右移 n 位 。类似无符号整数的右移，最低位丢弃。
     * 例如 a = "10100"
     * a >>= 2 之后，a 应该变成 "100"
     * a >>= 9 之后，a 应该变成 "" (即长度为 0)
     * @return 返回自身的引用
     */
    dynamic_bitset &operator >>= (std::size_t n) {
        if (n >= n_bits) {
            n_bits = 0;
            bits.clear();
            return *this;
        }
        if (n == 0) return *this;
        
        std::size_t word_shift = n / 64;
        std::size_t bit_shift = n % 64;
        
        std::size_t old_n_bits = n_bits;
        n_bits -= n;
        std::vector<uint64_t> new_bits((n_bits + 63) / 64, 0);
        
        for (std::size_t i = 0; i < new_bits.size(); ++i) {
            std::size_t src_idx = i + word_shift;
            uint64_t low = bits[src_idx] >> bit_shift;
            uint64_t high = 0;
            if (bit_shift > 0 && src_idx + 1 < bits.size()) {
                high = bits[src_idx + 1] << (64 - bit_shift);
            }
            new_bits[i] = low | high;
        }

        bits = std::move(new_bits);
        clean_unused_bits();
        return *this;
    }

    // 把所有位设置为 1
    dynamic_bitset &set() {
        for (auto &b : bits) b = ~uint64_t(0);
        clean_unused_bits();
        return *this;
    }
    // 把所有位取反
    dynamic_bitset &flip() {
        for (auto &b : bits) b = ~b;
        clean_unused_bits();
        return *this;
    }
    // 把所有位设置为 0
    dynamic_bitset &reset() {
        for (auto &b : bits) b = 0;
        return *this;
    }
    
    void clean_unused_bits() {
        if (n_bits > 0 && n_bits % 64 != 0) {
            bits.back() &= get_last_mask();
        } else if (n_bits == 0) {
            bits.clear();
        }
    }
};

} // namespace sjtu

#endif
