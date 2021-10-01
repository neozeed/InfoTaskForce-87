char c_getc(void){
char c;
#ifndef _WIN32
char *p;
p = (char*)0x800002;
c=*p;
#else
c=getchar();
#endif
return(c);
}