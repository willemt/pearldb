# -*- mode: python -*-
# vi: set ft=python :

import sys
import os


def options(opt):
    opt.load('compiler_c')


def configure(conf):
    conf.load('compiler_c')
    conf.load('clib')
    if sys.platform == 'darwin':
        conf.env.prepend_value('LINKFLAGS', ['-framework', 'CoreFoundation'])
        conf.env.prepend_value('LINKFLAGS', ['-framework', 'CoreServices'])


def build(bld):
    bld.load('clib')

    includes = []

    cflags = """
        -g
        -Werror=int-to-pointer-cast
        -Werror=unused-variable
        -Werror=return-type
        -Werror=uninitialized
        -Werror=pointer-to-int-cast
    """.split()

    lib = ['uv', 'h2o', 'ssl', 'crypto', 'ck']

    libpath = [os.getcwd()]

    if sys.platform == 'darwin':
        cflags.extend("""
            -fcolor-diagnostics
            -fdiagnostics-color
            """.split())

        includes.append('/usr/local/opt/openssl/include')
        libpath.append('/usr/local/opt/openssl/lib')

    elif sys.platform.startswith('linux'):
        lib.append('pthread')
        lib.append('rt')

    clibs = """
        b64
        bmon
        container_of
        h2o_helpers
        heap
        lmdb
        lmdb_helpers
        pidfile
        uv_helpers
        uv_multiplex
        """.split()

    h2o_includes = """
        ./deps/h2o/include
        ./deps/picohttpparser
        ./deps/klib
        ./deps/kstr
        """.split()

    uv_includes = """
        ./deps/libuv/include
        """.split()

    ck_includes = """
        ./deps/ck/include
        """.split()

    bld.program(
        source="""
        src/main.c
        """.split() + bld.clib_c_files(clibs),
        includes=['./include'] + includes + bld.clib_h_paths(clibs) + h2o_includes + uv_includes + ck_includes,
        target='pearl',
        stlibpath=['.'],
        libpath=libpath,
        lib=lib,
        cflags=cflags)
