#include <iostream>
#include <ostream>

// #pragma pack(4)  // 最大对齐粒度 4
#pragma pack()

struct MyStruct {
  int a;
  char b;
  double c;  // 最大对其粒度 8
};           // 16
inline std::ostream& operator<<(std::ostream& out, MyStruct obj) {
  out << "Size of MyStruct: " << sizeof(MyStruct) << '\n'
      << "Address of obj.a: " << &obj.a << std::endl
      << "Address of obj.b: " << static_cast<void*>(&obj.b) << std::endl
      << "Address of obj.c: " << &obj.c << std::endl;
  return out;
}

struct MyStruct2 {
  int a;
  double b;  // 最大对其粒度 8
  char c;
};  // 24
inline std::ostream& operator<<(std::ostream& out, MyStruct2 obj) {
  out << "Size of MyStruct2: " << sizeof(MyStruct2) << '\n'
      << "Address of obj.a: " << &(obj.a) << std::endl
      << "Address of obj.b: " << &(obj.b) << std::endl
      << "Address of obj.c: " << static_cast<void*>(&(obj.c)) << std::endl;
  return out;
}

struct MyStruct3 {
  int a;
  char b;
  double c;  // 最大对其粒度 8
  MyStruct o;
};  // 32
inline std::ostream& operator<<(std::ostream& out, MyStruct3 obj) {
  out << "Size of MyStruct3: " << sizeof(MyStruct3) << '\n'
      << "Address of obj.a: " << &(obj.a) << std::endl
      << "Address of obj.b: " << static_cast<void*>(&(obj.b)) << std::endl
      << "Address of obj.c: " << &(obj.c) << std::endl
      << "Address of obj.o: " << &(obj.o) << std::endl;
  return out;
}

struct MyStruct4 {
  int a;
  char b;
  double c;  // 最大对其粒度 8
  MyStruct2 o;
};  // 40
inline std::ostream& operator<<(std::ostream& out, MyStruct4 obj) {
  out << "Size of MyStruct4: " << sizeof(MyStruct4) << '\n'
      << "Address of obj.a: " << &(obj.a) << std::endl
      << "Address of obj.b: " << static_cast<void*>(&(obj.b)) << std::endl
      << "Address of obj.c: " << &(obj.c) << std::endl
      << "Address of obj.o: " << &(obj.o) << std::endl;
  return out;
}

struct MyStruct5 {
  int a;
  char b;
  double c;  // 最大对其粒度 8
  struct MyStruct6 {
    short _a;  // 最大对其粒度 2
    char _b;
  };  // 4
  MyStruct6 o;
};  // 24
inline std::ostream& operator<<(std::ostream& out, MyStruct5::MyStruct6 obj) {
  out << "Size of MyStruct6: " << sizeof(MyStruct5::MyStruct6) << '\n'
      << "Address of obj.a: " << &(obj._a) << std::endl
      << "Address of obj.b: " << static_cast<void*>(&(obj._b)) << std::endl;
  return out;
}

inline std::ostream& operator<<(std::ostream& out, MyStruct5 obj) {
  out << "Size of MyStruct5: " << sizeof(MyStruct5) << '\n'
      << "Address of obj.a: " << &(obj.a) << std::endl
      << "Address of obj.b: " << static_cast<void*>(&(obj.b)) << std::endl
      << "Address of obj.c: " << &(obj.c) << std::endl
      << obj.o;
  return out;
}