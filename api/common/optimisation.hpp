
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

#ifndef GUARD_2022_November_04_optimisation
#define GUARD_2022_November_04_optimisation

// NOTE: to use do following ( tldr use #pragma with the macro )
//
//  to turn off completely for translation unit just do:
//  #pragma DISABLE_GCC_OPTIMIZATIONS
//
//  or for scoped control to
//
//  #pragma PUSH_GCC_OPTIONS
//  #pragma DISABLE_GCC_OPTIMIZATIONS
//
//  foobar(); // unoptimised code here
//
//  #pragma POP_GCC_OPTIONS

#define PUSH_GCC_OPTIONS GCC push_options
#define DISABLE_GCC_OPTIMIZATIONS GCC optimize( 0 )     
#define POP_GCC_OPTIONS GCC pop_options

#endif //GUARD_2022_November_04_optimisation
