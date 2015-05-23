LIBUV_BRANCH=v1.2.1
LIBH2O_BRANCH=rel/v1.0.0

lets_build_this:
	python waf configure
	python waf build
.PHONY : default_target

clean:
	python waf clean
.PHONY : clean

libh2o:
	if test -e deps/h2o; \
	then cd deps/h2o && git pull origin $(LIBH2O_BRANCH); \
	else git clone https://github.com/h2o/h2o deps/h2o; \
	fi
	cd deps/h2o && git checkout $(LIBH2O_BRANCH)
	cd deps/h2o && cmake . -DCMAKE_INCLUDE_PATH=../libuv/include -DLIBUV_LIBRARIES=1
	cd deps/h2o && make libh2o
	cp deps/h2o/libh2o.a .
.PHONY : libh2o

libuv:
	if test -e deps/libuv; \
	then cd deps/libuv && git pull origin $(LIBUV_BRANCH); \
	else git clone https://github.com/libuv/libuv deps/libuv; \
	fi
	cd deps/libuv && git checkout $(LIBUV_BRANCH)
	cd deps/libuv && sh autogen.sh
	cd deps/libuv && ./configure
	cd deps/libuv && make
	cp deps/libuv/.libs/libuv.a .
.PHONY : libuv

usage.c:
	docopt2ragel USAGE > src/usage.rl
	ragel src/usage.rl
