FROM ubuntu:bionic

WORKDIR /app

RUN apt-get update          \
    && apt-get -y install   \
        curl

RUN curl -sL https://deb.nodesource.com/setup_10.x | bash - \
    && apt-get install -y nodejs

COPY ./package.json /app

RUN npm install -g serve    \
    && npm install          \
    && npm audit fix

COPY . /app

RUN npm run-script build

ENTRYPOINT [ "serve" ]
CMD [ "-p", "80", "-s", "./build" ]
