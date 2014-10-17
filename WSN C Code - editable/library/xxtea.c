/******************************************************************************\
 * This source code is released to the public domain.
\******************************************************************************/

/***************************************************************************//**
 * Implementation of XXTEA algorithm. XXTEA is also known as Corrected Block
 * TEA, where TEA is Tiny Encryption Algorithm.
 *
 * This code is based on public domain sample code:
 * @code
 * D.J. Wheeler, and R.M. Needham, Oct. 1998. Correction to xtea. Computer 
 * Laboratory, Cambridge University, England.
 * http://www.movable-type.co.uk/scripts/xxtea.pdf
 * @endcode
 *
 * @file xxtea.c
 * @date 17-Jan-2010
 ******************************************************************************/


#ifdef UC_8051
#   include "global.h"
#else
#   include <stdint.h>
#endif
#include "xxtea.h"


#define MX ((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z));


void xxtea(uint32_t* v, int8_t n, uint32_t const k[4])
{
    uint32_t z = v[n-1];
    uint32_t y = v[0];
    uint32_t sum = 0;
    uint32_t DELTA = 0x9e3779b9;
    uint8_t p;
    uint8_t q;
    uint8_t e;

    if (n > 1) {
        /* Coding Part */
        q = 6 + 52/n;
        while (q-- > 0) {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n-1; p++) {
                y = v[p+1], z = v[p] += MX
            }
            y = v[0];
            z = v[n-1] += MX
        }
    }
    else if (n < -1) {
        /* Decoding Part */
        n = -n;
        q = 6 + 52/n;
        sum = q * DELTA;
        while (sum != 0) {
            e = (sum >> 2) & 3;
            for (p = n-1; p > 0; p--) {
                z = v[p-1], y = v[p] -= MX
            }
            z = v[n-1];
            y = v[0] -= MX
            sum -= DELTA;
        }
    }
}

