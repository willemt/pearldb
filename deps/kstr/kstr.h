#ifndef KSTR_H
#define KSTR_H

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif /* min */

typedef struct
{
    size_t len;
    char* s;
} kstr_t;

/**
 * Compare kstr to NUL terminated C string
 * @param[in] a kstr
 * @param[in] b NUL terminated C string
 */
int kstrccmp(kstr_t* a, char* b)
{
    return strncmp(a->s, b, a->len);
}

/**
 * Compare kstr to a kstr
 * @param[in] a kstr
 * @param[in] b kstr
 */
int kstrcmp(kstr_t* a, kstr_t* b)
{
    return strncmp(a->s, b->s, min(a->len, b->len));
}

/**
 * djb2 by Dan Bernstein. */
unsigned long kstrhash(const void * o)
{
    const kstr_t* name = o;
    const char* str;
    unsigned long hash = 5381;
    int c;
    size_t i;

    for (i = 0,
         str = name->s; i < name->len && (c = *(str++));
         i++ )
        hash = ((hash << 5) + hash) + c;
    return hash;
}

#endif /* KSTR_H */
