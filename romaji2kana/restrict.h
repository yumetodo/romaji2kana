#ifndef INC_RESTRICT_H_
#define INC_RESTRICT_H_
//WE WILL NEVER SUPPORT C89 OR BEFORE!
#ifndef RESTRICT
#	if defined(_MSC_VER) || defined(__cplusplus)
#		define RESTRICT
#	else
#		define RESTRICT restrict
#	endif
#endif
#endif //INC_RESTRICT_H_
