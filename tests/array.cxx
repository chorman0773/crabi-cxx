#include <crabi/array.hxx>

#include "test.hxx"

int main(){
    crabi::array<int,0> x{};
    crabi::array y{1,2,3};
    crabi::array z{4,5,6};
    std::ranges::swap(y,z);

    auto [a,b,c] = y;

    assert_eq(a,4);
    assert_eq(b,5);
    assert_eq(c,6);
}