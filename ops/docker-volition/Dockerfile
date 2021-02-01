# Use an official Python runtime as a parent image
FROM ubuntu:bionic

WORKDIR /app

RUN apt-get update          \
    && apt-get -y --no-install-recommends install \
        ca-certificates     \
        curl                \
        gcc-7               \
        g++-7               \
        git                 \
        make                \
        openssl             \
        sqlite3             \
        libsqlite3-dev      \
        vim                 \
        valgrind

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 7 \
    &&  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 7 \
    &&  update-alternatives --auto gcc \
    &&  update-alternatives --auto g++ \
    &&  gcc -v

RUN git clone -b v3.14.4 https://gitlab.kitware.com/cmake/cmake.git \
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
    && make -j4 VERBOSE=1   \
    && make install         \
    && cd ..                \
    && rm -r poco

RUN curl -s https://packagecloud.io/install/repositories/immortal/immortal/script.deb.sh | bash \
    && apt install immortal

COPY . /app

RUN cmake .                 \
    && make volition -j4 VERBOSE=1   \
    && make install         \
    && /sbin/ldconfig -v    \
    && cd ..

ENTRYPOINT [ "volition" ]
CMD [ "-c", "/var/lib/volition/volition.ini" ]
