# Use an official Python runtime as a parent image
FROM ubuntu:bionic

WORKDIR /app

RUN apt-get update && \
    apt-get install -y \
        clang \
        cmake \
        git \
        libssl-dev \
        make \
        openssl

RUN git clone -b v3.14.4 https://gitlab.kitware.com/cmake/cmake.git

RUN cd cmake && \
    apt-get purge cmake -y && \
    ./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release && \
    make && \
    make install && \
    cd .. && \
    rm -r cmake

RUN git clone -b poco-1.9.1 https://github.com/pocoproject/poco && \
    cd poco && \
    cmake -DPOCO_ENABLE_SQL_MYSQL=OFF -DPOCO_ENABLE_SQL_POSTGRESQL=OFF -DPOCO_ENABLE_SQL_ODBC=OFF -DPOCO_ENABLE_NETSSL=ON . && \
    make install && \
    cd .. && \
    rm -r poco

COPY . /app

RUN ls -l && \
    cmake . && \
    make

RUN ls -al

CMD which cmake
