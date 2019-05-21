FROM ubuntu:bionic

WORKDIR /app

#ca-certificates     \

RUN    apt-get update       \
    && apt-get -y install   \
        nodejs              \
        npm

COPY ./package.json /app

RUN npm install

COPY . /app

ENTRYPOINT [ "node" ]
CMD [ "main.js", "dotenv_config_path=/var/lib/volition-market" ]
