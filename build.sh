mkdir -p _build && cd _build
cmake -DCMAKE_PREFIX_PATH=_deps/usr/local ..
make -j8
make DESTDIR=../_deps install
