# Receiving strings in constructors

This test compares different methods of receiving string parameters in constructors, taking into account all optimizations performed by major compilers.

The test counts the number of invocations of all constructors and assignment operators of a special class `DbgStr`.

Results:

[MSVC](msvc.txt) 

[GCC](gcc.txt)

[Clang](clang.txt)

The obtained results show that **the most efficient method of receiving string parameters in constructors is by creating a template constructor and perfect-forwarding string parameters to respective data members:**
```cpp
template<StringConvertible T>
UserPerfectFwd(T&& str) : name(std::forward<T>(str)) { }
```