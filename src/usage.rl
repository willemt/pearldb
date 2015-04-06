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
    int path;

    /* options */
    

    /* arguments */
    char* db_path;

} options_t;

struct params
{
    options_t* opt;
    char buffer[BUFLEN + 1];
    int buflen;
    int cs;
};

%%{
    machine params;
    access fsm->;

    action append {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = fc;
    }

    action term {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }

    action clear { fsm->buflen = 0; }

    
    action option_daemonize{ fsm->opt->daemonize = 1; }
    action option_help{ fsm->opt->help = 1; }
    action option_path{ fsm->opt->path = 1; }
    
    action argument_db_path{ fsm->opt->db_path = strdup(fsm->buffer); }

    string = [^\0] + > clear $append % term;

    main := (((((('-d' | '--daemonize') 0 @option_daemonize | 
(('-p' | '--path') 0 @option_path string 0 @argument_db_path)))?) | 
(('-h' | '--help') 0 @option_help)));
}%%

%% write data;

static void params_init(struct params *fsm, options_t* opt)
{
    fsm->opt = opt;
    fsm->buflen = 0;
    %% write init;
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    %% write exec;
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
    fprintf(stdout, "  peardb [--daemonize | --path <db_path>]\n");
    fprintf(stdout, "  peardb --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d --daemonize  Run as a daemon.\n");
    fprintf(stdout, "  -p --path  Path where database files will be kept [default: store]\n");
    fprintf(stdout, "  -h --help  Prints a short usage summary.\n");
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

