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
        curl

RUN git clone -b OpenSSL_1_1_1b https://github.com/openssl/openssl.git \
    && cd openssl           \
    && ./config             \
    && make                 \
    && make install

RUN curl -sL https://deb.nodesource.com/setup_10.x | bash - \
    && apt-get install -y nodejs

RUN npm install \
    && npm install -g log.io

EXPOSE 80
EXPOSE 6689
ENTRYPOINT [ "log.io-server" ]
