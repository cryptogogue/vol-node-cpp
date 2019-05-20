# Use an official Python runtime as a parent image
FROM ubuntu:bionic

WORKDIR /app

RUN    apt-get update       \
    && apt-get -y --no-install-recommends install \
        ca-certificates     \
        gcc                 \
        g++                 \
        git                 \
        make                \
        openssl             \
                            \
    && git clone -b v3.14.4 https://gitlab.kitware.com/cmake/cmake.git \
    && cd cmake             \
    && ./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release \
    && make -j4             \
    && make install         \
    && cd ..                \
    && rm -r cmake

RUN git clone -b OpenSSL_1_1_1b https://github.com/openssl/openssl.git \
    && cd openssl           \
    && ./config             \
    && make                 \
    && make install

RUN git clone -b poco-1.9.1 https://github.com/pocoproject/poco \
    && cd poco              \
    && cmake                \
        -DDISABLE_CPP11=ON  \
        -DDISABLE_CPP14=ON  \
        -DENABLE_NETSSL=ON  \
        -DENABLE_MONGODB=OFF \
        -DENABLE_REDIS=OFF  \
        -DENABLE_PDF=OFF    \
        -DENABLE_DATA=OFF   \
        -DENABLE_DATA_SQLITE=OFF \
        -DENABLE_DATA_MYSQL=OFF \
        -DENABLE_DATA_POSTGRESQL=OFF \
        -DENABLE_DATA_ODBC=OFF \
        -DENABLE_PAGECOMPILER=OFF \
        -DENABLE_PAGECOMPILER_FILE2PAGE=OFF \
        .                   \
    && make -j4             \
    && make install         \
    && cd ..                \
    && rm -r poco

RUN    apt-get update       \
    && apt-get -y --no-install-recommends install \
        valgrind

COPY . /app

RUN cmake .                 \
    && make -j4 VERBOSE=1   \
    && make install         \
    && /sbin/ldconfig -v    \
    && cd ..
#    && rm -r app

ENTRYPOINT [ "volition" ]
CMD [ "-p", "9090", "-s", "true", "-k", "/var/lib/volition/keys/pkey0.priv.json", "-g", "/var/lib/volition/genesis.signed" ]
