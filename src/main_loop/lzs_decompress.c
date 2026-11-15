#pragma peephole off
#include "types.h"

int decompress_lz(void *src, void *dest)
{
    u8 ringBuf[4096 + 18 - 1];
    u32 offset;
    int length;
    int i;
    u32 byte;
    u8 *destp;
    u32 bufPos;
    u8 *srcp;
    u32 flags;
    u32 srcSize;
    u32 destSize;

    srcSize = __lwbrx(src, 0);
    destSize = __lwbrx(src, 4);
    if (srcSize == 0 || destSize == 0)
        return 0;

    srcp = (u8 *)src + 8;
    destp = dest;

    for (offset = 0; (int)offset < 4078; offset++)
        ringBuf[offset] = 0;

    bufPos = 4078;
    flags = 0;
    while (1)
    {
        flags >>= 1;

        if (!(flags & 0x100))
        {
            byte = *srcp;
            if (srcp - (u8 *)src >= srcSize)
                break;
            srcp++;
            flags = byte | 0xFF00;
        }
        if (flags & 1)
        {
            byte = *srcp;
            if (srcp - (u8 *)src >= srcSize)
                break;
            srcp++;
#ifndef __MWERKS__
            /* PC: never write past the header's size (see below) */
            if ((u32)(destp - (u8 *)dest) >= destSize)
                break;
#endif
            *destp++ = byte;
            ringBuf[bufPos++] = byte;
            bufPos %= 4096;
        }
        else
        {
            offset = *srcp;
            if (srcp - (u8 *)src >= srcSize)
                break;
            srcp++;
            length = *srcp;
            if (srcp - (u8 *)src >= srcSize)
                break;
            srcp++;

            offset |= (length & 0xF0) << 4;
            length = (length & 0xF) + 2;

            for (i = 0; i <= length; i++)
            {
#ifndef __MWERKS__
                /* PC: some community stage tools end with a match running a
                 * few bytes past the size in the header; on the GC those
                 * bytes land in the next heap chunk's header (the packed
                 * data, freed right after), here they are dropped */
                if ((u32)(destp - (u8 *)dest) >= destSize)
                    break;
#endif
                byte = ringBuf[(offset + i) % 4096];
                *destp++ = byte;
                ringBuf[bufPos++] = byte;
                bufPos %= 4096;
            }
        }
    }

    return destSize;
}
