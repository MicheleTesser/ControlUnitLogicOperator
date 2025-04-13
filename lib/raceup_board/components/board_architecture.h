#ifndef __BOARD_ARCHITECTURE__
#define __BOARD_ARCHITECTURE__

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH 64
#elif defined(__i386__) || defined(_M_IX86) || __tricore__
#define ARCH 32
#else
#error "architecture not supported"
#endif

#endif // !__BOARD_ARCHITECTURE__
