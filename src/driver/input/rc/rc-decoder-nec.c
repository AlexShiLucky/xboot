/*
 * driver/input/rc-decoder-nec.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <input/rc/rc-decoder-nec.h>

enum {
    NEC_UNIT            = 560,
    NEC_HEADER_PULSE    = 560 * 16,
    NEC_HEADER_SPACE    = 560 * 8,
    NEC_REPEAT_SPACE    = 560 * 4,
    NEC_BIT_PULSE       = 560 * 1,
    NEC_BIT_0_SPACE     = 560 * 1,
    NEC_BIT_1_SPACE     = 560 * 3,
    NEC_TRAILER_PULSE   = 560 * 1,
    NEC_TRAILER_SPACE   = 560 * 10,
};

static const unsigned char byte_rev_table[256] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

static inline unsigned char bitrev8(unsigned char byte)
{
    return byte_rev_table[byte];
}

static inline int eq_margin(int d1, int d2, int margin)
{
    return ((d1 > (d2 - margin)) && (d1 < (d2 + margin))) ? 1 : 0;
}

static inline int geq_margin(int d1, int d2, int margin)
{
    return (d1 > (d2 - margin)) ? 1 : 0;
}

uint32_t rc_nec_decoder_handle(struct rc_decoder_nec_t * decoder, int pulse, int duration)
{
    uint8_t addr, raddr, cmd, rcmd;
    uint32_t code = 0;

    switch(decoder->state)
    {
    case RC_STATE_NEC_INACTIVE:
        if(!pulse)
            break;
        if(!eq_margin(duration, NEC_HEADER_PULSE, NEC_UNIT * 2))
            break;
        decoder->count = 0;
        decoder->state = RC_STATE_NEC_HEADER_SPACE;
        break;

    case RC_STATE_NEC_HEADER_SPACE:
        if(pulse)
            break;
        if(eq_margin(duration, NEC_HEADER_SPACE, NEC_UNIT))
        {
            decoder->repeat = 0;
            decoder->state = RC_STATE_NEC_BIT_PULSE;
        }
        else if(eq_margin(duration, NEC_REPEAT_SPACE, NEC_UNIT / 2))
        {
            decoder->repeat++;
            decoder->state = RC_STATE_NEC_TRAILER_PULSE;
        }
        else
        {
            decoder->state = RC_STATE_NEC_INACTIVE;
        }
        break;

    case RC_STATE_NEC_BIT_PULSE:
        if(!pulse)
            break;
        if(!eq_margin(duration, NEC_BIT_PULSE, NEC_UNIT / 2))
            break;
        decoder->state = RC_STATE_NEC_BIT_SPACE;
        break;

    case RC_STATE_NEC_BIT_SPACE:
        if(pulse)
            break;
        decoder->bits <<= 1;
        if(eq_margin(duration, NEC_BIT_1_SPACE, NEC_UNIT / 2))
            decoder->bits |= 1;
        else if(!eq_margin(duration, NEC_BIT_0_SPACE, NEC_UNIT / 2))
            break;
        if(++decoder->count == 32)
            decoder->state = RC_STATE_NEC_TRAILER_PULSE;
        else
            decoder->state = RC_STATE_NEC_BIT_PULSE;
        break;

    case RC_STATE_NEC_TRAILER_PULSE:
        if(!pulse)
            break;
        if(!eq_margin(duration, NEC_TRAILER_PULSE, NEC_UNIT / 2))
            break;
        decoder->state = RC_STATE_NEC_TRAILER_SPACE;
        break;

    case RC_STATE_NEC_TRAILER_SPACE:
        if(pulse)
            break;
        if(!geq_margin(duration, NEC_TRAILER_SPACE, NEC_UNIT / 2))
            break;
        if(decoder->repeat == 0 || decoder->repeat >= 5)
        {
            addr = bitrev8((decoder->bits >> 24) & 0xff);
            raddr = bitrev8((decoder->bits >> 16) & 0xff);
            cmd = bitrev8((decoder->bits >> 8) & 0xff);
            rcmd = bitrev8((decoder->bits >> 0) & 0xff);
            if((cmd ^ rcmd) != 0xff)
                code = decoder->bits;
            else if((addr ^ raddr) != 0xff)
                code = addr << 16 | raddr << 8 | cmd;
            else
                code = addr << 8 | cmd;
        }
        decoder->state = RC_STATE_NEC_INACTIVE;
        break;

    default:
        decoder->state = RC_STATE_NEC_INACTIVE;
        break;
    }

    return code;
}
