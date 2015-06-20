#include <stdio.h>
#include <string.h>

typedef struct {
    int get_keys;
    kstr_t key;
} parse_result_t;

struct path_parse
{
    parse_result_t* r;
    int cs;
};

%%{
    machine path_parse;
    access fsm->;

    action key_start {
        fsm->r->key.s = (char*)fpc;
    }

    action key_end {
        fsm->r->key.len = (size_t)(fpc - fsm->r->key.s);
    }

    action getkeys {
        fsm->r->get_keys = 1;
    }

    unreserved  = alnum | "-" | "." | "_" | "~" | "=";

    key = unreserved+ > key_start % key_end;

    main := ('/' key '/'?) |
            ("/key/" key '/'? %getkeys) |
            ("/key/"  %getkeys);

}%%

%% write data;

static void pp_init(struct path_parse *fsm, parse_result_t* result)
{
    fsm->r = result;
    fsm->r->get_keys = 0;

    %% write init;
}

static void pp_execute(struct path_parse *fsm, const char *data, size_t len)
{
    const char *p = data;
    const char *pe = data + len;
    const char *eof = data + len;

    %% write exec;
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

