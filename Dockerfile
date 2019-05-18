# Use an official Python runtime as a parent image
FROM ubuntu:bionic

WORKDIR /app

RUN    apt-get update       \
    && apt-get -y --no-install-recommends install \
        ca-certificates     \
        clang               \
        git                 \
        libssl-dev          \
        make                \
        openssl             \
                            \
    && git clone -b v3.14.4 https://gitlab.kitware.com/cmake/cmake.git \
    && cd cmake             \
    && ./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release \
    && make -j4             \
    && make install         \
    && cd ..                \
    && rm -r cmake          \
                            \
    && git clone -b poco-1.9.1 https://github.com/pocoproject/poco \
    && cd poco              \
    && cmake -DPOCO_ENABLE_SQL_MYSQL=OFF -DPOCO_ENABLE_SQL_POSTGRESQL=OFF -DPOCO_ENABLE_SQL_ODBC=OFF -DPOCO_ENABLE_NETSSL=ON . \
    && make -j4             \
    && make install         \
    && cd ..                \
    && rm -r poco

COPY . /app

RUN    cmake .              \
    && make -j4             \
    && make install         \
    && cd ..                \
    && rm -r app            \
    && /sbin/ldconfig -v

ENTRYPOINT [ "volition" ]
CMD [ "-p", "9090", "-s", "true", "-k", "/var/lib/volition/keys/pkey0.priv.json", "-g", "/var/lib/volition/genesis.signed" ]
