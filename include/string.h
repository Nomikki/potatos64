#ifndef STRING_H_
#define STRING_H_

#include <types.h>

extern int strlen(const char *str);
extern char *strcpy(char *destination, const char *source);

/*
  Copy block of memory
  Copies the values of num bytes from the location pointed to
  by source directly to the memory block pointed to by destination.
*/
extern void *memcpy(void *destination, const void *source, size_t num);

/*
  Move block of memory
  Copies the values of num bytes from the location pointed by
  source to the memory block pointed by destination. Copying takes
  place as if an intermediate buffer were used, allowing the destination
  and source to overlap.
*/
extern void *memmove(void *destination, const void *source, size_t num);

/*
  Fill block of memory
  Sets the first num bytes of the block of memory pointed by ptr to
  the specified value(interpreted as an unsigned char)
*/
extern void *memset(void *ptr, int value, size_t num);

#endif // STRING_H_