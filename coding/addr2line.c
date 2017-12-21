#include <stdio.h>

#define TELLME printf ("This is line %d of file \"%s\" (function <%s>)\n",\
                      __LINE__, __FILE__, __func__)

#line 1000 "thousand"
void whereAmI ()
{
    TELLME;
}

#line 999 "almost_one_thousand"
int main ()
{
    TELLME;
    whereAmI();

#line 1 "reset_line"
    TELLME;
    whereAmI();
    return 0;
}

