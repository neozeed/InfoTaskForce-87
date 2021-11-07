unsigned char memory_zone[256*1024];
int memory_offset;

#ifdef _WIN32
void memory_init(){}

#else
void * 	malloc (int len)
{
void *p;
//printf("malloc %d offset is %d ",len,memory_offset);
p=&memory_zone[memory_offset];
memory_offset+=(len+10);
//printf("offset is now %d\n",memory_offset);
return(p);
}

void 	free (void *p)
{}

//http://www.ethernut.de/api/memset_8c_source.html
 void *memset(void *dst, int c, int n)
 {
     if (n) {
         char *d = dst;
 
         do {
             *d++ = c;
         } while (--n);
     }
     return dst;
 }

void memory_init(){
memset(memory_zone,0x0,sizeof(memory_zone));
memory_offset=0;
}
#endif
