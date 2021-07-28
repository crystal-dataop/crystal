# Crystal Data OP library

Crystal is a data storage, analysis and retrieval library. The purpose is to
solve the analysis and retrieval of data in big data scenarios.

- scalable and easy-to-use data analysis op library
- support kv storage
- support bitmap and vector(faiss) index
- supply an HTTP+JSON request and response service demo

In progress:

- more data analysis op
- more data source support (such as CSV, HDF5, arrow, other database)
- python binding
- ...

Build & test:

Crystal requires c++17, tested on MacOS 10.15 currently, Linux will be tested
soon.

```sh
brew install asio double-conversion gflags omp  # for mac

./deps.sh
./build.sh
cd _build && make test
```

Website & docs:

- English: https://cdataop.com/en/
- 中文: https://cdataop.com

