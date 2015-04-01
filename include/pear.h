#ifndef PEAR_H
#define PEAR_H

/**
 * Copyright (c) 2015, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

typedef struct
{
    MDB_dbi docs;
    MDB_env *db_env;
    h2o_globalconf_t cfg;
} server_t;

extern server_t server;
extern server_t *sv;

#endif /* PEAR_H */
