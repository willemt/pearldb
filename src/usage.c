
#line 1 "src/usage.rl"
#include <stdio.h>
#include <string.h>

#define BUFLEN 1024
#define BUFSIZE 2048

typedef struct
{
    /* commands */
    int stat;

    /* flags */
    int daemonize;
    int help;

    /* options */
    char* batch_period;
    char* db_size;
    char* host;
    char* path;
    char* pid_file;
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


#line 78 "src/usage.rl"



#line 46 "src/usage.c"
static const char _params_actions[] = {
	0, 1, 0, 1, 3, 1, 4, 1, 
	5, 2, 1, 6, 2, 1, 7, 2, 
	1, 8, 2, 1, 9, 2, 1, 10, 
	2, 1, 11, 2, 1, 12, 2, 2, 
	0
};

static const char _params_key_offsets[] = {
	0, 0, 10, 15, 16, 17, 18, 19, 
	20, 21, 22, 23, 24, 25, 26, 27, 
	28, 29, 38, 43, 45, 46, 47, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 
	57, 58, 59, 60, 61, 62, 63, 64, 
	65, 66, 67, 70, 71, 72, 73, 74, 
	75, 76, 77, 78, 79, 80, 81, 82, 
	83, 84, 85, 86, 87, 88, 89, 90, 
	91, 92, 93, 94, 95, 96, 97, 98, 
	100, 101, 102, 103, 104, 105, 106, 107, 
	109, 110
};

static const char _params_trans_keys[] = {
	45, 97, 98, 100, 104, 105, 112, 115, 
	116, 119, 98, 100, 104, 112, 119, 97, 
	116, 99, 104, 95, 112, 101, 114, 105, 
	111, 100, 0, 0, 0, 45, 97, 98, 
	100, 105, 112, 115, 116, 119, 98, 100, 
	104, 112, 119, 97, 98, 101, 109, 111, 
	110, 105, 122, 101, 0, 95, 115, 105, 
	122, 101, 0, 0, 0, 111, 115, 116, 
	0, 0, 0, 97, 105, 111, 116, 104, 
	0, 0, 0, 100, 95, 102, 105, 108, 
	101, 0, 0, 0, 114, 116, 0, 0, 
	0, 111, 114, 107, 101, 114, 115, 0, 
	0, 0, 101, 111, 108, 112, 0, 116, 
	97, 116, 0, 45, 115, 45, 0
};

static const char _params_single_lengths[] = {
	0, 10, 5, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 9, 5, 2, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 3, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 2, 
	1, 1, 1, 1, 1, 1, 1, 2, 
	1, 0
};

static const char _params_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0
};

static const short _params_index_offsets[] = {
	0, 0, 11, 17, 19, 21, 23, 25, 
	27, 29, 31, 33, 35, 37, 39, 41, 
	43, 45, 55, 61, 64, 66, 68, 70, 
	72, 74, 76, 78, 80, 82, 84, 86, 
	88, 90, 92, 94, 96, 98, 100, 102, 
	104, 106, 108, 112, 114, 116, 118, 120, 
	122, 124, 126, 128, 130, 132, 134, 136, 
	138, 140, 142, 144, 146, 148, 150, 152, 
	154, 156, 158, 160, 162, 164, 166, 168, 
	171, 173, 175, 177, 179, 181, 183, 185, 
	188, 190
};

static const char _params_trans_targs[] = {
	2, 45, 14, 27, 74, 54, 59, 33, 
	39, 68, 0, 3, 19, 71, 42, 62, 
	0, 4, 0, 5, 0, 6, 0, 7, 
	0, 8, 0, 9, 0, 10, 0, 11, 
	0, 12, 0, 13, 0, 14, 0, 15, 
	0, 0, 16, 80, 16, 18, 45, 14, 
	27, 54, 59, 33, 39, 68, 0, 3, 
	19, 36, 42, 62, 0, 20, 28, 0, 
	21, 0, 22, 0, 23, 0, 24, 0, 
	25, 0, 26, 0, 27, 0, 80, 0, 
	29, 0, 30, 0, 31, 0, 32, 0, 
	33, 0, 34, 0, 0, 35, 80, 35, 
	37, 0, 38, 0, 39, 0, 40, 0, 
	0, 41, 80, 41, 43, 48, 57, 0, 
	44, 0, 45, 0, 46, 0, 0, 47, 
	80, 47, 49, 0, 50, 0, 51, 0, 
	52, 0, 53, 0, 54, 0, 55, 0, 
	0, 56, 80, 56, 58, 0, 59, 0, 
	60, 0, 0, 61, 80, 61, 63, 0, 
	64, 0, 65, 0, 66, 0, 67, 0, 
	68, 0, 69, 0, 0, 70, 80, 70, 
	72, 37, 0, 73, 0, 74, 0, 81, 
	0, 76, 0, 77, 0, 78, 0, 81, 
	0, 1, 75, 0, 17, 0, 0, 0
};

static const char _params_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 30, 9, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 5, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 30, 12, 1, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 30, 15, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 30, 
	18, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 30, 21, 1, 0, 0, 0, 0, 
	0, 0, 0, 30, 24, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 30, 27, 1, 
	0, 0, 0, 0, 0, 0, 0, 7, 
	0, 0, 0, 0, 0, 0, 0, 3, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const int params_start = 79;
static const int params_first_final = 79;
static const int params_error = 0;

static const int params_en_main = 79;


#line 81 "src/usage.rl"

static void params_init(struct params *fsm, options_t* opt)
{
    memset(opt, 0, sizeof(options_t));

    fsm->opt = opt;
    fsm->buflen = 0;
    fsm->opt->batch_period = strdup("50000");
    fsm->opt->db_size = strdup("1000");
    fsm->opt->host = strdup("127.0.0.1");
    fsm->opt->path = strdup("store");
    fsm->opt->pid_file = strdup("/var/run/pearl.pid");
    fsm->opt->port = strdup("8888");
    fsm->opt->workers = strdup("8");

    
#line 206 "src/usage.c"
	{
	 fsm->cs = params_start;
	}

#line 97 "src/usage.rl"
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    
#line 220 "src/usage.c"
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
#line 42 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = (*p);
    }
	break;
	case 1:
#line 47 "src/usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }
	break;
	case 2:
#line 52 "src/usage.rl"
	{ fsm->buflen = 0; }
	break;
	case 3:
#line 54 "src/usage.rl"
	{ fsm->opt->stat = 1; }
	break;
	case 4:
#line 55 "src/usage.rl"
	{ fsm->opt->daemonize = 1; }
	break;
	case 5:
#line 56 "src/usage.rl"
	{ fsm->opt->help = 1; }
	break;
	case 6:
#line 57 "src/usage.rl"
	{ fsm->opt->batch_period = strdup(fsm->buffer); }
	break;
	case 7:
#line 58 "src/usage.rl"
	{ fsm->opt->db_size = strdup(fsm->buffer); }
	break;
	case 8:
#line 59 "src/usage.rl"
	{ fsm->opt->host = strdup(fsm->buffer); }
	break;
	case 9:
#line 60 "src/usage.rl"
	{ fsm->opt->path = strdup(fsm->buffer); }
	break;
	case 10:
#line 61 "src/usage.rl"
	{ fsm->opt->pid_file = strdup(fsm->buffer); }
	break;
	case 11:
#line 62 "src/usage.rl"
	{ fsm->opt->port = strdup(fsm->buffer); }
	break;
	case 12:
#line 63 "src/usage.rl"
	{ fsm->opt->workers = strdup(fsm->buffer); }
	break;
#line 351 "src/usage.c"
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

#line 105 "src/usage.rl"
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
    fprintf(stdout, "pearldb - a key value server\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "  pearldb [--daemonize | -a DB_PATH | -t HOST | -p PORT | -w WORKERS | -b NANOS | -s MEGAS | -i PID_FILE]\n");
    fprintf(stdout, "  pearldb stat\n");
    fprintf(stdout, "  pearldb --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d --daemonize           Run as a daemon.\n");
    fprintf(stdout, "  -a --path DB_PATH        Path where database files will be kept [default: store]\n");
    fprintf(stdout, "  -t --host HOST           Host to listen on [default: 127.0.0.1]\n");
    fprintf(stdout, "  -p --port PORT           Port to listen on [default: 8888]\n");
    fprintf(stdout, "  -w --workers WORKERS     Number of worker threads [default: 8]\n");
    fprintf(stdout, "  -b --batch_period NANOS  Number of nano seconds between batch commits [default: 50000]\n");
    fprintf(stdout, "  -s --db_size MEGAS       Size of database in megabytes [default: 1000]\n");
    fprintf(stdout, "  -i --pid_file PID_FILE   Pid file [default: /var/run/pearl.pid]\n");
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

