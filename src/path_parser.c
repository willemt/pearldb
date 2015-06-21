
#line 1 "src/path_parser.rl"
#include <stdio.h>
#include <string.h>

typedef struct
{
    int get_keys;
    kstr_t key;
} parse_result_t;

struct path_parse
{
    parse_result_t* r;
    int cs;
};


#line 33 "src/path_parser.rl"



#line 24 "src/path_parser.c"
static const char _path_parse_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 2, 
	1, 2, 2, 3, 2
};

static const char _path_parse_key_offsets[] = {
	0, 0, 1, 13, 23, 23, 34, 45, 
	55, 65, 75, 75
};

static const char _path_parse_trans_keys[] = {
	47, 61, 95, 107, 126, 45, 46, 48, 
	57, 65, 90, 97, 122, 47, 61, 95, 
	126, 45, 57, 65, 90, 97, 122, 47, 
	61, 95, 101, 126, 45, 57, 65, 90, 
	97, 122, 47, 61, 95, 121, 126, 45, 
	57, 65, 90, 97, 122, 47, 61, 95, 
	126, 45, 57, 65, 90, 97, 122, 47, 
	61, 95, 126, 45, 57, 65, 90, 97, 
	122, 47, 61, 95, 126, 45, 57, 65, 
	90, 97, 122, 0
};

static const char _path_parse_single_lengths[] = {
	0, 1, 4, 4, 0, 5, 5, 4, 
	4, 4, 0, 0
};

static const char _path_parse_range_lengths[] = {
	0, 0, 4, 3, 0, 3, 3, 3, 
	3, 3, 0, 0
};

static const char _path_parse_index_offsets[] = {
	0, 0, 2, 11, 19, 20, 29, 38, 
	46, 54, 62, 63
};

static const char _path_parse_indicies[] = {
	0, 1, 2, 2, 3, 2, 2, 2, 
	2, 2, 1, 5, 4, 4, 4, 4, 
	4, 4, 1, 1, 5, 4, 4, 6, 
	4, 4, 4, 4, 1, 5, 4, 4, 
	7, 4, 4, 4, 4, 1, 8, 4, 
	4, 4, 4, 4, 4, 1, 10, 9, 
	9, 9, 9, 9, 9, 1, 12, 11, 
	11, 11, 11, 11, 11, 1, 1, 1, 
	0
};

static const char _path_parse_trans_targs[] = {
	2, 0, 3, 5, 3, 4, 6, 7, 
	8, 9, 11, 9, 10
};

static const char _path_parse_trans_actions[] = {
	0, 0, 1, 1, 0, 3, 0, 0, 
	3, 1, 0, 0, 3
};

static const char _path_parse_eof_actions[] = {
	0, 0, 0, 3, 0, 3, 3, 3, 
	0, 7, 5, 10
};

static const int path_parse_start = 1;
static const int path_parse_first_final = 3;
static const int path_parse_error = 0;

static const int path_parse_en_main = 1;


#line 36 "src/path_parser.rl"

static void pp_init(struct path_parse *fsm, parse_result_t* result)
{
    fsm->r = result;
    fsm->r->get_keys = 0;
    
#line 104 "src/path_parser.c"
	{
	 fsm->cs = path_parse_start;
	}

#line 42 "src/path_parser.rl"
}

static void pp_execute(struct path_parse *fsm, const char *data, size_t len)
{
    const char *p = data;
    const char *pe = data + len;
    const char *eof = data + len;
    
#line 118 "src/path_parser.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if (  fsm->cs == 0 )
		goto _out;
_resume:
	_keys = _path_parse_trans_keys + _path_parse_key_offsets[ fsm->cs];
	_trans = _path_parse_index_offsets[ fsm->cs];

	_klen = _path_parse_single_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _path_parse_range_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _path_parse_indicies[_trans];
	 fsm->cs = _path_parse_trans_targs[_trans];

	if ( _path_parse_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _path_parse_actions + _path_parse_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 20 "src/path_parser.rl"
	{ fsm->r->key.s = (char*)p; }
	break;
	case 1:
#line 21 "src/path_parser.rl"
	{ fsm->r->key.len = (size_t)(p - fsm->r->key.s); }
	break;
#line 200 "src/path_parser.c"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _path_parse_actions + _path_parse_eof_actions[ fsm->cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 1:
#line 21 "src/path_parser.rl"
	{ fsm->r->key.len = (size_t)(p - fsm->r->key.s); }
	break;
	case 2:
#line 22 "src/path_parser.rl"
	{ fsm->r->get_keys = 1; }
	break;
	case 3:
#line 23 "src/path_parser.rl"
	{ fsm->r->key.len = 0; }
	break;
#line 228 "src/path_parser.c"
		}
	}
	}

	_out: {}
	}

#line 50 "src/path_parser.rl"
}

static int pp_finish(struct path_parse *fsm)
{
    if (fsm->cs == path_parse_error)
        return -1;
    if (fsm->cs >= path_parse_first_final)
        return 1;
    return 0;
}

int parse_path(const char *path, size_t len, parse_result_t *result)
{
    struct path_parse pp;
    pp_init(&pp, result);
    pp_execute(&pp, path, len);
    if (pp_finish(&pp) != 1)
        return -1;
    return 0;
}
