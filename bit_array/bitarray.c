/*
**  Functions to maintain an arbitrary length array of bits
**
**  Public domain by Bob Stout
**
**  Note: On some targets where pointers may not align on byte bouncries,
**        a different bas type than char will be rewuired. This is left as
**        an exercize for the reader.
*/

#include "bitops.h"

/*
**  Static function to calculate index and offset, given bit position
*/

static void idx_calc(size_t posn, size_t *idx, size_t *ofs)
{
      *idx = posn / CHAR_BIT;       /* Byte index [0-N] in the array    */
      *ofs = posn % CHAR_BIT;       /* Bit number [0-N] within the byte */
}

BYTE *alloc_bit_array(size_t bits)
{
      BYTE *set = calloc((bits + CHAR_BIT - 1) / CHAR_BIT, sizeof(BYTE));

      return set;
}

int getbit(BYTE *set, int number)
{
      size_t idx, ofs;

      idx_calc(number, &idx, &ofs);
      set += idx;
      return (*set & (1 << ofs)) != 0;                      /* 0 or 1   */
}

void setbit(BYTE *set, int number, int value)
{
      size_t idx, ofs;

      idx_calc(number, &idx, &ofs);
      set += idx;
      if (value)
            *set |= 1 << ofs;                               /* set bit  */
      else  *set &= ~(1 << ofs);                            /* clear bit*/
}

void flipbit(BYTE *set, int number)
{
      size_t idx, ofs;

      idx_calc(number, &idx, &ofs);
      set += idx;
      *set ^= 1 << ofs;                                     /* flip bit */
}

#ifdef TEST

#include <stdio.h>
#include <stddef.h>

int main(int argc, char *argv[])
{
      BYTE *Array;
      int i, n = argc - 1;
      int *vals;

      if (NULL == (Array = alloc_bit_array(64)))
      {
            puts("Unable to allocate bit array");
            return EXIT_FAILURE;
      }
      else  printf("Array %d words x %d bits\n\n", sizeof(Array)/sizeof(BYTE), CHAR_BIT * sizeof(BYTE));

      for (i = 0; --argc; ++i)
            vals[i] = atoi(*(++argv));

      printf(" Array = %02X%02X%02X%02X%02X%02X%02X%02X\n",
             Array[7],  Array[6],  Array[5],  Array[4], Array[3],  Array[2],  Array[1],  Array[0]);
      for (i = 0; i < n; ++i)
      {
            setbit(Array, vals[i], 1);
            printf("After setting bit %d,\n"
                   " Array = %02X%02X%02X%02X%02X%02X%02X%02X\n", vals[i],
                   Array[7],  Array[6],  Array[5],  Array[4], Array[3],  Array[2],  Array[1],  Array[0]);
      }

      printf(" Array = %02X%02X%02X%02X%02X%02X%02X%02X\n",
             Array[7],  Array[6],  Array[5],  Array[4], Array[3],  Array[2],  Array[1],  Array[0]);
      for (i = 0; i < n; ++i)
      {
            setbit(Array, vals[i], 0);
            printf("After clearing bit %d,\n"
                   " Array = %02X%02X%02X%02X%02X%02X%02X%02X\n", vals[i],
                   Array[7],  Array[6],  Array[5],  Array[4], Array[3],  Array[2],  Array[1],  Array[0]);
      }

      return EXIT_SUCCESS;
}

#endif // TEST
