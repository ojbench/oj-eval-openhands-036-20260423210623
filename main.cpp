

#include "dynamic_bitset.h"
#include <cassert>
#include <iostream>
#include <string>

void test_basic() {
    dynamic_bitset db(10);
    assert(db.size() == 10);
    for (int i = 0; i < 10; ++i) assert(db[i] == false);
    
    db.set(3, true);
    assert(db[3] == true);
    db.set(3, false);
    assert(db[3] == false);
    
    db.push_back(true);
    assert(db.size() == 11);
    assert(db[10] == true);
}

void test_string_init() {
    dynamic_bitset db("0010");
    assert(db.size() == 4);
    assert(db[0] == false);
    assert(db[1] == false);
    assert(db[2] == true);
    assert(db[3] == false);
}

void test_none_all() {
    dynamic_bitset db(10);
    assert(db.none() == true);
    assert(db.all() == false);
    
    db.set(5);
    assert(db.none() == false);
    assert(db.all() == false);
    
    db.set();
    assert(db.none() == false);
    assert(db.all() == true);
    
    db.reset();
    assert(db.none() == true);
    assert(db.all() == false);
}

void test_bitwise() {
    dynamic_bitset a("10101");
    dynamic_bitset b("1100");
    
    dynamic_bitset a_or = a;
    a_or |= b;
    // a = "10101", b = "1100"
    // a | b = "11101"
    assert(a_or.size() == 5);
    assert(a_or[0] == true);
    assert(a_or[1] == true);
    assert(a_or[2] == true);
    assert(a_or[3] == false);
    assert(a_or[4] == true);
    
    dynamic_bitset b_or = b;
    b_or |= a;
    // b | a = "1110"
    assert(b_or.size() == 4);
    assert(b_or[0] == true);
    assert(b_or[1] == true);
    assert(b_or[2] == true);
    assert(b_or[3] == false);
}

void test_shift() {
    dynamic_bitset a("1110");
    a <<= 3;
    // a = "0001110"
    assert(a.size() == 7);
    assert(a[0] == false);
    assert(a[1] == false);
    assert(a[2] == false);
    assert(a[3] == true);
    assert(a[4] == true);
    assert(a[5] == true);
    assert(a[6] == false);
    
    dynamic_bitset b("10100");
    b >>= 2;
    // b = "100"
    assert(b.size() == 3);
    assert(b[0] == true);
    assert(b[1] == false);
    assert(b[2] == false);
    
    dynamic_bitset c("10100");
    c >>= 9;
    assert(c.size() == 0);
}

int main() {
    test_basic();
    test_string_init();
    test_none_all();
    test_bitwise();
    test_shift();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

