void c_putc(char c){
#ifndef _WIN32
char *p;
p = (char*)0x800001;
*p=c;
#else
putchar(c);
#endif
}