
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
    char* db_size;
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


#line 71 "src/usage.rl"



#line 44 "src/usage.c"
static const char _params_actions[] = {
	0, 1, 0, 1, 3, 1, 4, 2, 
	1, 5, 2, 1, 6, 2, 1, 7, 
	2, 1, 8, 2, 1, 9, 2, 2, 
	0
};

static const char _params_key_offsets[] = {
	0, 0, 8, 13, 14, 15, 16, 17, 
	18, 19, 20, 21, 22, 23, 24, 25, 
	26, 27, 34, 38, 40, 41, 42, 43, 
	44, 45, 46, 47, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 58, 59, 60, 
	61, 62, 63, 64, 65, 66, 67, 68, 
	69, 70, 71, 72, 73, 74, 75, 76, 
	77, 78, 79, 80, 81, 82, 83
};

static const char _params_trans_keys[] = {
	45, 97, 98, 100, 104, 112, 115, 119, 
	98, 100, 104, 112, 119, 97, 116, 99, 
	104, 95, 112, 101, 114, 105, 111, 100, 
	0, 0, 0, 45, 97, 98, 100, 112, 
	115, 119, 98, 100, 112, 119, 97, 98, 
	101, 109, 111, 110, 105, 122, 101, 0, 
	95, 115, 105, 122, 101, 0, 0, 0, 
	97, 111, 116, 104, 0, 0, 0, 114, 
	116, 0, 0, 0, 111, 114, 107, 101, 
	114, 115, 0, 0, 0, 101, 108, 112, 
	0, 45, 45, 0
};

static const char _params_single_lengths[] = {
	0, 8, 5, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 7, 4, 2, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
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
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _params_index_offsets[] = {
	0, 0, 9, 15, 17, 19, 21, 23, 
	25, 27, 29, 31, 33, 35, 37, 39, 
	41, 43, 51, 56, 59, 61, 63, 65, 
	67, 69, 71, 73, 75, 77, 79, 81, 
	83, 85, 87, 89, 91, 94, 96, 98, 
	100, 102, 104, 106, 108, 110, 112, 114, 
	116, 118, 120, 122, 124, 126, 128, 130, 
	132, 134, 136, 138, 140, 142, 144
};

static const char _params_indicies[] = {
	0, 2, 3, 4, 5, 6, 7, 8, 
	1, 9, 10, 11, 12, 13, 1, 14, 
	1, 15, 1, 16, 1, 17, 1, 18, 
	1, 19, 1, 20, 1, 21, 1, 22, 
	1, 23, 1, 3, 1, 24, 1, 1, 
	25, 27, 26, 28, 2, 3, 4, 6, 
	7, 8, 1, 9, 10, 12, 13, 1, 
	29, 30, 1, 31, 1, 32, 1, 33, 
	1, 34, 1, 35, 1, 36, 1, 4, 
	1, 37, 1, 38, 1, 39, 1, 40, 
	1, 41, 1, 7, 1, 42, 1, 1, 
	43, 45, 44, 46, 47, 1, 48, 1, 
	2, 1, 49, 1, 1, 50, 52, 51, 
	53, 1, 6, 1, 54, 1, 1, 55, 
	57, 56, 58, 1, 59, 1, 60, 1, 
	61, 1, 62, 1, 8, 1, 63, 1, 
	1, 64, 66, 65, 67, 1, 68, 1, 
	5, 1, 69, 1, 70, 1, 71, 1, 
	1, 0
};

static const char _params_trans_targs[] = {
	2, 0, 39, 14, 27, 59, 44, 33, 
	53, 3, 19, 56, 36, 47, 4, 5, 
	6, 7, 8, 9, 10, 11, 12, 13, 
	15, 16, 16, 61, 18, 20, 28, 21, 
	22, 23, 24, 25, 26, 61, 29, 30, 
	31, 32, 34, 35, 35, 61, 37, 42, 
	38, 40, 41, 41, 61, 43, 45, 46, 
	46, 61, 48, 49, 50, 51, 52, 54, 
	55, 55, 61, 57, 58, 62, 1, 17
};

static const char _params_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 22, 1, 7, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 3, 0, 0, 
	0, 0, 0, 22, 1, 10, 0, 0, 
	0, 0, 22, 1, 13, 0, 0, 22, 
	1, 16, 0, 0, 0, 0, 0, 0, 
	22, 1, 19, 0, 0, 5, 0, 0
};

static const int params_start = 60;
static const int params_first_final = 60;
static const int params_error = 0;

static const int params_en_main = 60;


#line 74 "src/usage.rl"

static void params_init(struct params *fsm, options_t* opt)
{
    memset(opt, 0, sizeof(options_t));

    fsm->opt = opt;
    fsm->buflen = 0;
    fsm->opt->batch_period = strdup("50000");
    fsm->opt->db_size = strdup("1000");
    fsm->opt->path = strdup("store");
    fsm->opt->port = strdup("8888");
    fsm->opt->workers = strdup("4");

    
#line 178 "src/usage.c"
	{
	 fsm->cs = params_start;
	}

#line 88 "src/usage.rl"
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    
#line 192 "src/usage.c"
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
#line 40 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = (*p);
    }
	break;
	case 1:
#line 45 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }
	break;
	case 2:
#line 50 "src/usage.rl"
	{ fsm->buflen = 0; }
	break;
	case 3:
#line 53 "src/usage.rl"
	{ fsm->opt->daemonize = 1; }
	break;
	case 4:
#line 54 "src/usage.rl"
	{ fsm->opt->help = 1; }
	break;
	case 5:
#line 55 "src/usage.rl"
	{ fsm->opt->batch_period = strdup(fsm->buffer); }
	break;
	case 6:
#line 56 "src/usage.rl"
	{ fsm->opt->db_size = strdup(fsm->buffer); }
	break;
	case 7:
#line 57 "src/usage.rl"
	{ fsm->opt->path = strdup(fsm->buffer); }
	break;
	case 8:
#line 58 "src/usage.rl"
	{ fsm->opt->port = strdup(fsm->buffer); }
	break;
	case 9:
#line 59 "src/usage.rl"
	{ fsm->opt->workers = strdup(fsm->buffer); }
	break;
#line 312 "src/usage.c"
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

#line 96 "src/usage.rl"
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
    fprintf(stdout, "  peardb [--daemonize | -a DB_PATH | -p PORT | -w WORKERS | -b NANOS | -s MEGAS]\n");
    fprintf(stdout, "  peardb --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d --daemonize           Run as a daemon.\n");
    fprintf(stdout, "  -a --path DB_PATH        Path where database files will be kept [default: store]\n");
    fprintf(stdout, "  -p --port PORT           Port to listen on [default: 8888]\n");
    fprintf(stdout, "  -w --workers WORKERS     Number of worker threads [default: 4]\n");
    fprintf(stdout, "  -b --batch_period NANOS  Number of nano seconds between batch commits [default: 50000]\n");
    fprintf(stdout, "  -s --db_size MEGAS       Size of database in megabytes [default: 1000]\n");
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

