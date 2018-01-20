#include "Arduino.h"

char *ftoa(char *a, double f, int precision) {
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long decimal = abs((long)((f - heiltal) * p[precision]));
 itoa(decimal, a, 10);
 return ret;
}


