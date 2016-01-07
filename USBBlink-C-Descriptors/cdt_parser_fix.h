/// This file is purely to stop the CDT parser throwing up a fuckton of errors when it meets the SDCC
/// compiler. This file should not affect the build since __CDT_PARSER__ isn't defined.

#ifdef __CDT_PARSER__
#define __crticial
#define __xdata
#define __at(x)
#define __code
#define __sfr int
#define __sbit int
#define __interrupt(x)
#define __bit int
#endif
