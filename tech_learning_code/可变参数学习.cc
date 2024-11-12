/////////////////////////////////
//可变参数宏函数
// #include <stdio.h>

// #define LOG(fmt,...) printf("[%s-%d]"fmt,__FILE__,__LINE__,##__VA_ARGS__)

// int main(int argc, char *argv[]) {
//     LOG("hello\n");

//     return 0;
// }



//c风格可变参数函数
// #include <stdio.h>
// #include <stdarg.h>
// #include<stdlib.h>
// void myprint(const char* fmt, ...)
// {
//     va_list ap;
//     va_start(ap, fmt);
//     char *buffer;
//     vasprintf(&buffer,fmt,ap);
    
//     printf("%s",buffer);
//     va_end(ap);
//     free(buffer);
// }

// int main()
// {
//     myprint("%s - %d\n","李四",1); // 输出：李四 - 1
//     return 0;
// }
