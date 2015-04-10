
#line 1 "src/usage.rl"
#include <stdio.h>
#include <string.h>

#define BUFLEN 1024
#define BUFSIZE 2048

typedef struct
{
    /* commands */
    

    /* flags */
    int daemonize;
    int help;

    /* options */
    char* path;

    /* arguments */
    

} options_t;

struct params
{
    options_t* opt;
    char buffer[BUFLEN + 1];
    int buflen;
    int cs;
};


#line 59 "src/usage.rl"



#line 40 "src/usage.c"
static const char _params_actions[] = {
	0, 1, 0, 1, 1, 1, 2
};

static const char _params_key_offsets[] = {
	0, 0, 4, 7, 8, 9, 10, 11, 
	12, 13, 14, 15, 16, 17, 18, 19, 
	20, 21, 22, 23, 24, 25
};

static const char _params_trans_keys[] = {
	45, 100, 104, 112, 100, 104, 112, 97, 
	101, 109, 111, 110, 105, 122, 101, 0, 
	101, 108, 112, 0, 97, 116, 104, 0, 
	45, 0
};

static const char _params_single_lengths[] = {
	0, 4, 3, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 0
};

static const char _params_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0
};

static const char _params_index_offsets[] = {
	0, 0, 5, 9, 11, 13, 15, 17, 
	19, 21, 23, 25, 27, 29, 31, 33, 
	35, 37, 39, 41, 43, 45
};

static const char _params_trans_targs[] = {
	2, 11, 15, 19, 0, 3, 12, 16, 
	0, 4, 0, 5, 0, 6, 0, 7, 
	0, 8, 0, 9, 0, 10, 0, 11, 
	0, 21, 0, 13, 0, 14, 0, 15, 
	0, 21, 0, 17, 0, 18, 0, 19, 
	0, 21, 0, 1, 0, 0, 0
};

static const char _params_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 3, 0, 0, 0, 0, 0, 0, 
	0, 5, 0, 0, 0, 0, 0
};

static const int params_start = 20;
static const int params_first_final = 20;
static const int params_error = 0;

static const int params_en_main = 20;


#line 62 "src/usage.rl"

static void params_init(struct params *fsm, options_t* opt)
{
    memset(opt, 0, sizeof(options_t));

    fsm->opt = opt;
    fsm->buflen = 0;
    fsm->opt->path = strdup("store");

    
#line 112 "src/usage.c"
	{
	 fsm->cs = params_start;
	}

#line 72 "src/usage.rl"
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    
#line 126 "src/usage.c"
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
	_keys = _params_trans_keys + _params_key_offsets[ fsm->cs];
	_trans = _params_index_offsets[ fsm->cs];

	_klen = _params_single_lengths[ fsm->cs];
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

	_klen = _params_range_lengths[ fsm->cs];
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
	 fsm->cs = _params_trans_targs[_trans];

	if ( _params_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _params_actions + _params_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 49 "src/usage.rl"
	{ fsm->opt->daemonize = 1; }
	break;
	case 1:
#line 50 "src/usage.rl"
	{ fsm->opt->help = 1; }
	break;
	case 2:
#line 51 "src/usage.rl"
	{ fsm->opt->path = strdup(fsm->buffer); }
	break;
#line 211 "src/usage.c"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 80 "src/usage.rl"
}

static int params_finish(struct params *fsm)
{
    if (fsm->cs == params_error)
        return -1;
    if (fsm->cs >= params_first_final)
        return 1;
    return 0;
}

static void show_usage()
{
    fprintf(stdout, "peardb - a key value server\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "  peardb [--daemonize | --path DB_PATH]\n");
    fprintf(stdout, "  peardb --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d --daemonize       Run as a daemon.\n");
    fprintf(stdout, "  -p --path DB_PATH    Path where database files will be kept [default: store]\n");
    fprintf(stdout, "  -h --help            Prints a short usage summary.\n");
    fprintf(stdout, "\n");
}

static int parse_options(int argc, char **argv, options_t* options)
{
    int a;
    struct params params;

    params_init(&params, options);
    for (a = 1; a < argc; a++ )
        params_execute(&params, argv[a], strlen(argv[a]) + 1);
    if (params_finish(&params) != 1)
    {
        fprintf(stderr, "Error processing arguments\n");
        show_usage();
        return -1;
    }

    return 0;
}

