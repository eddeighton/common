
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "common/clang_tsan_memcpy.h"

#include <string.h>

// void* __tsan_memset( void* s, int c, size_t count ) __attribute__( ( alias( "memset" ) ) );
// void* __tsan_memmove( void* dst, const void* src, size_t len ) __attribute__( ( alias( "memmove" ) ) );
// void* __tsan_memcpy( void* dst, const void* src, size_t len ) __attribute__( ( alias( "memcpy" ) ) );

void* __tsan_memset( void* s, int c, size_t count ) 
{
    return memset( s, c, count );
}
void* __tsan_memmove( void* dst, const void* src, size_t len )
{
    return memmove( dst, src, len );
}
void* __tsan_memcpy( void* dst, const void* src, size_t len )
{
    return memcpy( dst, src, len );
}

