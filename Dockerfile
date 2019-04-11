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
        openssl \
    && \
    git clone https://github.com/pocoproject/poco && \
    cd poco && \
    cmake -DPOCO_ENABLE_SQL_MYSQL=OFF -DPOCO_ENABLE_SQL_POSTGRESQL=OFF -DPOCO_ENABLE_SQL_ODBC=OFF -DPOCO_ENABLE_NETSSL=ON && \
    make install && \
    cd .. && \
    rm -r poco

COPY . /app

RUN ls -l && \
    cmake . && \
    make install

# Run app.py when the container launches
CMD which cmake
