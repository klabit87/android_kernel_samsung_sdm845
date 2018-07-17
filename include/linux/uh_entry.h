/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _UH_ENTRY_H
#define _UH_ENTRY_H

#ifndef __ASSEMBLY__
#if 0
typedef unsigned long long u64;
typedef unsigned int       u32;
typedef unsigned short     u16;
typedef unsigned char      u8;
typedef signed long long   s64;
typedef signed int         s32;
typedef signed short       s16;
typedef signed char        s8;
#endif


#define	APP_INIT	0
#define	APP_SAMPLE	1

#define UH_PREFIX  UL(0xc300c000)
#define UH_APPID(APP_ID)  ((UL(APP_ID) & UL(0xFF)) | UH_PREFIX)

#define UH_APP_INIT		UH_APPID(APP_INIT)
#define UH_APP_SAMPLE	UH_APPID(APP_SAMPLE)

int uh_call(u64 app_id, u64 command, u64 arg0, u64 arg1, u64 arg2, u64 arg3);

#endif //__ASSEMBLY__

#endif //_UH_ENTRY_H
