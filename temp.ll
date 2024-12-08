; ModuleID = 'lightir'
source_filename = "lightir"
target triple = "x86_64-pc-linux-gnu"

%string = type { i32, i8* }

@tigermain_framesize = external global i64
@tigermain_framesize.1 = external constant i64

declare void @flush()

declare void @exit(i32)

declare %string* @chr(i32)

declare %string* @__wrap_getchar()

declare void @print(%string*)

declare void @printi(i32)

declare i32 @ord(%string*)

declare i32 @size(%string*)

declare %string* @concat(%string*, %string*)

declare %string* @substring(%string*, i32, i32)

declare i64 @alloc_record(i32)

declare i64 @init_array(i32, i64)

declare i1 @string_equal(%string*, %string*)

define i32 @tigermain(i64 %0, i64 %1) {
tigermain:
  %2 = load i64, i64* @tigermain_framesize.1, align 4
  %3 = sub i64 %0, %2
  %4 = load i64, i64* @tigermain_framesize.1, align 4
  %5 = add i64 %4, -8
  %6 = add i64 %3, %5
  %7 = inttoptr i64 %6 to i32*
  store i32 8, i32* %7, align 4
  %8 = load i64, i64* @tigermain_framesize.1, align 4
  %9 = add i64 %3, %8
  %10 = add i64 %9, -8
  %11 = inttoptr i64 %10 to i32*
  %12 = load i32, i32* %11, align 4
  %array_ptr = call i64 @init_array(i32 %12, i64 0)
}
