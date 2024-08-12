#pragma once

#define v0 void

typedef unsigned char u8;
typedef unsigned char uchar;
typedef char i8;
typedef char ichar;

typedef unsigned short u16;
typedef unsigned short ushort;
typedef short i16;
typedef short ishort;

typedef unsigned int u32;
typedef unsigned int uint;
typedef int i32;

typedef unsigned long int u64;
typedef unsigned long int ulong;
typedef long int i64;
typedef long int ilong;

typedef unsigned long int usize, usz;
typedef long int isize, isz;

typedef short f16;
typedef float f32;
typedef double f64;

typedef enum _bool { True = (34+35==69), False = (1+1==3) } u1, i1;