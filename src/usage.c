
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
    char* batch_period;
    char* path;
    char* port;
    char* workers;

    /* arguments */
    

} options_t;

struct params
{
    options_t* opt;
    char buffer[BUFLEN + 1];
    int buflen;
    int cs;
};


#line 68 "src/usage.rl"



#line 43 "src/usage.c"
static const char _params_actions[] = {
	0, 1, 0, 1, 3, 1, 4, 2, 
	1, 5, 2, 1, 6, 2, 1, 7, 
	2, 1, 8, 2, 2, 0
};

static const char _params_key_offsets[] = {
	0, 0, 7, 12, 13, 14, 15, 16, 
	17, 18, 19, 20, 21, 22, 23, 24, 
	25, 26, 32, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 47, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 
	58, 59, 60, 61, 62, 63, 64, 65, 
	66, 67, 68, 69, 70, 71, 72
};

static const char _params_trans_keys[] = {
	45, 97, 98, 100, 104, 112, 119, 98, 
	100, 104, 112, 119, 97, 116, 99, 104, 
	95, 112, 101, 114, 105, 111, 100, 0, 
	0, 0, 45, 97, 98, 100, 112, 119, 
	98, 100, 112, 119, 97, 101, 109, 111, 
	110, 105, 122, 101, 0, 97, 111, 116, 
	104, 0, 0, 0, 114, 116, 0, 0, 
	0, 111, 114, 107, 101, 114, 115, 0, 
	0, 0, 101, 108, 112, 0, 45, 45, 
	0
};

static const char _params_single_lengths[] = {
	0, 7, 5, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 6, 4, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 0
};

static const char _params_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _params_index_offsets[] = {
	0, 0, 8, 14, 16, 18, 20, 22, 
	24, 26, 28, 30, 32, 34, 36, 38, 
	40, 42, 49, 54, 56, 58, 60, 62, 
	64, 66, 68, 70, 72, 75, 77, 79, 
	81, 83, 85, 87, 89, 91, 93, 95, 
	97, 99, 101, 103, 105, 107, 109, 111, 
	113, 115, 117, 119, 121, 123, 125
};

static const char _params_indicies[] = {
	0, 2, 3, 4, 5, 6, 7, 1, 
	8, 9, 10, 11, 12, 1, 13, 1, 
	14, 1, 15, 1, 16, 1, 17, 1, 
	18, 1, 19, 1, 20, 1, 21, 1, 
	22, 1, 3, 1, 23, 1, 1, 24, 
	26, 25, 27, 2, 3, 4, 6, 7, 
	1, 8, 9, 11, 12, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 1, 
	33, 1, 34, 1, 4, 1, 35, 1, 
	36, 37, 1, 38, 1, 2, 1, 39, 
	1, 1, 40, 42, 41, 43, 1, 6, 
	1, 44, 1, 1, 45, 47, 46, 48, 
	1, 49, 1, 50, 1, 51, 1, 52, 
	1, 7, 1, 53, 1, 1, 54, 56, 
	55, 57, 1, 58, 1, 5, 1, 59, 
	1, 60, 1, 61, 1, 1, 0
};

static const char _params_trans_targs[] = {
	2, 0, 31, 14, 27, 51, 36, 45, 
	3, 19, 48, 28, 39, 4, 5, 6, 
	7, 8, 9, 10, 11, 12, 13, 15, 
	16, 16, 53, 18, 20, 21, 22, 23, 
	24, 25, 26, 53, 29, 34, 30, 32, 
	33, 33, 53, 35, 37, 38, 38, 53, 
	40, 41, 42, 43, 44, 46, 47, 47, 
	53, 49, 50, 54, 1, 17
};

static const char _params_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	19, 1, 7, 0, 0, 0, 0, 0, 
	0, 0, 0, 3, 0, 0, 0, 0, 
	19, 1, 10, 0, 0, 19, 1, 13, 
	0, 0, 0, 0, 0, 0, 19, 1, 
	16, 0, 0, 5, 0, 0
};

static const int params_start = 52;
static const int params_first_final = 52;
static const int params_error = 0;

static const int params_en_main = 52;


#line 71 "src/usage.rl"

static void params_init(struct params *fsm, options_t* opt)
{
    memset(opt, 0, sizeof(options_t));

    fsm->opt = opt;
    fsm->buflen = 0;
    fsm->opt->batch_period = strdup("50000");
    fsm->opt->path = strdup("store");
    fsm->opt->port = strdup("8888");
    fsm->opt->workers = strdup("4");

    
#line 165 "src/usage.c"
	{
	 fsm->cs = params_start;
	}

#line 84 "src/usage.rl"
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    
#line 179 "src/usage.c"
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
	_trans = _params_indicies[_trans];
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
#line 39 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = (*p);
    }
	break;
	case 1:
#line 44 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }
	break;
	case 2:
#line 49 "src/usage.rl"
	{ fsm->buflen = 0; }
	break;
	case 3:
#line 52 "src/usage.rl"
	{ fsm->opt->daemonize = 1; }
	break;
	case 4:
#line 53 "src/usage.rl"
	{ fsm->opt->help = 1; }
	break;
	case 5:
#line 54 "src/usage.rl"
	{ fsm->opt->batch_period = strdup(fsm->buffer); }
	break;
	case 6:
#line 55 "src/usage.rl"
	{ fsm->opt->path = strdup(fsm->buffer); }
	break;
	case 7:
#line 56 "src/usage.rl"
	{ fsm->opt->port = strdup(fsm->buffer); }
	break;
	case 8:
#line 57 "src/usage.rl"
	{ fsm->opt->workers = strdup(fsm->buffer); }
	break;
#line 295 "src/usage.c"
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

#line 92 "src/usage.rl"
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
    fprintf(stdout, "  peardb [--daemonize | -a DB_PATH | -p PORT | -w WORKERS | -b NANOS]\n");
    fprintf(stdout, "  peardb --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d --daemonize           Run as a daemon.\n");
    fprintf(stdout, "  -a --path DB_PATH        Path where database files will be kept [default: store]\n");
    fprintf(stdout, "  -p --port PORT           Port to listen on [default: 8888]\n");
    fprintf(stdout, "  -w --workers WORKERS     Number of worker threads [default: 4]\n");
    fprintf(stdout, "  -b --batch_period NANOS  Number of nano seconds between batch commits [default: 50000]\n");
    fprintf(stdout, "  -h --help                Prints a short usage summary.\n");
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

