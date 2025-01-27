#ifndef TOKEN_H
#define TOKEN_H

/* Returns static memory or null */
/* ----------------------------- */
char *token_get( char **s );

/* Returns s */
/* --------- */
char *token_skip_space( char *s );

#endif
