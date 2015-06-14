LIBUV_BRANCH=v1.2.1
LIBH2O_BRANCH=rel/v1.0.0

lets_build_this:
	python waf configure
	python waf build
.PHONY : default_target

clean:
	python waf clean || true
	cd deps/ck && make clean > /dev/null
	cd deps/libuv && make clean > /dev/null
	cd deps/h2o && make clean > /dev/null
.PHONY : clean

libh2o_build:
	cd deps/h2o && git checkout $(LIBH2O_BRANCH)
	cd deps/h2o && cmake . -DCMAKE_INCLUDE_PATH=../libuv/include -DLIBUV_LIBRARIES=1
	cd deps/h2o && make libh2o
	cp deps/h2o/libh2o.a .
.PHONY : libh2o_build

libh2o_fetch:
	if test -e deps/h2o; \
	then cd deps/h2o && rm -f CMakeCache.txt && git pull origin $(LIBH2O_BRANCH) ; \
	else git clone https://github.com/h2o/h2o deps/h2o; \
	fi
.PHONY : libh2o_fetch

libh2o: libh2o_fetch libh2o_build
.PHONY : libh2o

libuv_build:
	cd deps/libuv && sh autogen.sh
	cd deps/libuv && ./configure
	cd deps/libuv && make
	cp deps/libuv/.libs/libuv.a .
.PHONY : libuv_build

libuv_fetch:
	if test -e deps/libuv; \
	then cd deps/libuv && git pull origin $(LIBUV_BRANCH); \
	else git clone https://github.com/libuv/libuv deps/libuv; \
	fi
	cd deps/libuv && git checkout $(LIBUV_BRANCH)
.PHONY : libuv_fetch

libuv: libuv_fetch libuv_build
.PHONY : libuv

libck_build:
	cd deps/ck && ./configure
	cd deps/ck && make
	cp deps/ck/src/libck.a .
.PHONY : libck_build

libck_fetch:
	if test -e deps/ck; \
	then cd deps/ck && git pull ; \
	else git clone http://github.com/concurrencykit/ck deps/ck; \
	fi
.PHONY : libck_fetch

libck: libck_fetch libck_build
.PHONY : libck

libck_vendor:
	rm -rf deps/ck/.git > /dev/null
.PHONY : libck_vendor

libuv_vendor:
	rm -rf deps/uv/.git > /dev/null
.PHONY : libuv_vendor

libh2o_vendor:
	rm -rf deps/h2o/.git > /dev/null
.PHONY : libh2o_vendor

vendordeps: libck_vendor libuv_vendor libh2o_vendor
.PHONY : vendordeps

usage.c:
	docopt2ragel USAGE > src/usage.rl
	ragel src/usage.rl
