letsbuildthis:
	python waf configure
	python waf build

clean:
	python waf clean

install_libuv:
	if test -e libuv; \
	then cd libuv && git pull ; \
	else git clone http://github.com/joyent/libuv; \
	fi
	cd libuv && git checkout v0.10
	cd libuv && make
	mkdir -p build
	cp libuv/libuv.a .


