/* library/validation_level.h */
/* -------------------------- */

#ifndef VALIDATION_LEVEL_H
#define VALIDATION_LEVEL_H

enum validation_level { provisional, validated, either };

enum validation_level validation_level_get_validation_level(
				char *validation_level_string );

enum validation_level validation_level_string_resolve(
				char *validation_level_string );

char *validation_level_get_string( enum validation_level );

char *validation_level_display( enum validation_level validation_level );

char *validation_level_get_title_string(
				enum validation_level validation_level );

#endif

