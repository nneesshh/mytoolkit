#ifndef PLAT_ASCIIOPS_H
#define PLAT_ASCIIOPS_H

#include <stdint.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool int
#endif

/// Get numeric value of character (this includes hex value)
extern s32 digitValue(s8 asc);

/// Ascii-only common functions
extern bool hasCase(s8 asc);
extern bool isUpper(s8 asc);
extern bool isLower(s8 asc);
extern bool isTitle(s8 asc);
extern bool isAscii(s8 asc);
extern bool isLetter(s8 asc);
extern bool isDigit(s8 asc);
extern bool isAlphaNumeric(s8 asc);
extern bool isControl(s8 asc);
extern bool isSpace(s8 asc);
extern bool isBlank(s8 asc);
extern bool isPunct(s8 asc);
extern bool isGraph(s8 asc);
extern bool isPrint(s8 asc);
extern bool isHexDigit(s8 asc);
extern bool isSymbol(s8 asc);
extern bool isSep(s8 asc);

/// Case conversion
extern s8 toUpper(s8 asc);
extern s8 toLower(s8 asc);
extern s8 toTitle(s8 asc);

#ifdef __cplusplus
}
#endif

#endif // PLAT_ASCIIOPS_H
