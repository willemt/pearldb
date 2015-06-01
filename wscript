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

    cflags = """
         -g
        -Werror=int-to-pointer-cast
        -Werror=unused-variable
        -Werror=return-type
        -Werror=uninitialized
        -Werror=pointer-to-int-cast
    """.split()

    lib = ['uv', 'h2o', 'ssl', 'crypto']

    if sys.platform == 'darwin':
        cflags.extend("""
            -fcolor-diagnostics
            -fdiagnostics-color
            """.split())
    elif sys.platform.startswith('linux'):
        lib.append('pthread')
        lib.append('rt')

    clibs = """
        lmdb
        lmdb_helpers
        container_of
        uv_multiplex
        uv_helpers
        bmon
        heap
        b64
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

    bld.program(
        source="""
        src/main.c
        """.split() + bld.clib_c_files(clibs),
        includes=['./include'] + bld.clib_h_paths(clibs) + h2o_includes + uv_includes,
        target='pear',
        stlibpath=['.'],
        libpath=[os.getcwd()],
        lib=lib,
        cflags=cflags)
