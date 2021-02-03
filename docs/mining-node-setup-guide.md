# Mining Node Setup Guide

Volition is meant to run on a dedicated server or a reasonably-powered VPS. The current implementation is  gated more by bandiwdth and storage than CPU or RAM, though long-term performance with larger mining networks remains to be seen.

This guide assumes you are running Ubuntu or a similar flavor of Linux. This is not (yet) an exhaustive guide; you may still need to search and troubleshoot.

Also, before you begin, make sure you have an active account on whatever Volition network you plan to join and have the genesis block for that network. When you start mining, you will need someone to upgrade your regular account to a mining account. You may also want to provision a separate account just for mining.

### Prepare Your Server

To get started, you'll want to install:

```
docker
docker-compose
git
openssl
```

You will may also want to install curl and a text editor (such as vi/vim).

Finally, check your server's settings and make sure port 80 is open. You will also need to open port 443 if you plan to configure HTTPS/SSL.

### Clone and Build the Node

First, clone the repository from github:

```
git clone --recurse-submodules git@github.com:cryptogogue/vol-node-cpp.git
```

Now build the docker image using a helper script:

```
cd vol-node-cpp
cp ./ops/.env.example ./ops/.env
./ops/build.sh
```

If you look inside build.sh, you'll see it generates a header file containing some information from git, then it invokes docker to build the image.

The .env file has some settings you don't probably need to change. It is ignored by git; you may want to adjust it later in certain advanced scenarios (if you need to deploy multiple versions of the node on the same machine, for example).

### Set Up the Docker Volume

The reference Docker configuration maps ./ops/volume-volition onto /var/lib/volition/ inside the Docker container. This folder should be configured to contain the volition.ini file and the genesis block. We'll add the keys in a separate step.

The miner will need the genesis block for the network you want to join. If the genesis block you need is hosted somewhere, you can use curl to pull it into the folder:

```
curl <URL of genesis block> -o ./ops/volume-volition/genesis.json
```

Now, copy and configure volition.ini:

```
cp ./ops/volume-volition/volition.ini.example ./ops/volume-volition/volition.ini
```

And edit the file by adding the name of the account you plan to use for mining.

```
control-key             = /var/lib/volition/keys/control.pub.pem
control-level           = config
genesis                 = /var/lib/volition/genesis.json
keyfile                 = /var/lib/volition/keys/mining.priv.pem
logpath                 = /var/lib/volition/log
persist                 = /var/lib/volition/persist-chain
port                    = 9090

miner                   = <your miner account name>
```

### Generate Your Keys

Create a folder for the keys:

```
mkdir ./ops/volume-volition/keys
cd ./ops/volume-volition/keys
```

You will need a mining key and a control key. The mining key will is a 4096-bit RSA key used to identify your mining node and sign blocks. The control key is a secp256k1 elliptic key used to sign mining node control transactions.

You can generate the keys using the openssl command-line utility. Here are commands to generate the mining key pair and export the public key (which we'll need later to register the minig account):

```
openssl genrsa -out mining.priv.pem 4096
openssl rsa -in mining.priv.pem -outform PEM -pubout -out mining.pub.pem
```

And here are commands to generate the secp256k1 elliptic "miner control" key:

```
openssl ecparam -genkey -name secp256k1 -conv_form compressed -noout -out control.priv.pem.tmp
openssl pkcs8 -in control.priv.pem.tmp -topk8 -nocrypt -out control.priv.pem
openssl ec -in control.priv.pem -outform PEM -pubout -out control.pub.pem
rm control.priv.pem.tmp
```

Store these keys somewhere safe. On the server hosting the node you will need the *private* mining key and the *public* control key. To upgrade your account for mining you will need the *public* mining key and to sign miner control transactions (from your wallet) you will need the *private* control key.

When you are done, return to the project root directory:

### Run the Mining Node

```
./ops/compose-volition/up.sh
```

This will run the mining node. Open your web browser and navigate to your server's IP address or domain. You should see a .json dump of information about the node.

### Configuring HTTPS/SSL

To use HTTPS/SSL you will need a domain name. Procuring a domain name is outside of the scope of this document; check with your hosting provider for instructions.

One you have a domain name, there are two ways to set up HTTPS. The first is to produre an SSL certificate, install it manually and configure the node to use it by editing volition.ini. The work in this approach is procuring and maintaining the certificate. The configuration is fairly straightforward to do; we'll update this document later with instructions.

For now, we recommend you use nginx and certbot to via docker-compose to automatically obtain a letsencrypt SSL certiciate.

First, copy the example nginx configuration directory in volume-nginx:

```
cp volume-nginx.example volume-nginx
```

Now rename the example configuration file to your domain:

```
mv www.mydomain.com.conf <mysubdomain>.<mydomain>.<mydomainextension>.conf
```

For example, if you own the domain 'volhodler.net' and you want to access the node at the subdomain 'node':

```
mv www.mydomain.com.conf node.volhodler.net.conf
```

Now, open the conf file and edit it to include your domain in the SSL path. Note that the domain must be formatted correctly for certbot to work:

```
server {
    listen 443              ssl;
    server_name             <mysubdomain>.<mydomain>.<mydomainextension>;
    ssl_certificate         /etc/letsencrypt/live/<mysubdomain>.<mydomain>.<mydomainextension>/fullchain.pem;
    ssl_certificate_key     /etc/letsencrypt/live/<mysubdomain>.<mydomain>.<mydomainextension>/privkey.pem;

    location / {
        proxy_pass http://volition:80;
    }
}
```

In our example from above, this would be:

```
server {
    listen 443              ssl;
    server_name             node.volhodler.net;
    ssl_certificate         /etc/letsencrypt/live/node.volhodler.net/fullchain.pem;
    ssl_certificate_key     /etc/letsencrypt/live/node.volhodler.net/privkey.pem;

    location / {
        proxy_pass http://volition:80;
    }
}
```

Once that is done, make sure the original instance of volition is stopped:

```
./ops/compose-volition/down.sh
```

Now you can run volition with nginx:

```
./ops/compose-volition-nginx/up.sh
```

Navigate to the domain you configured and you should see the SSL lock appear in the address bar to indicate a secure site.

### Configure VOLWAL

If you're using VOLWAL as your wallet, open it in your browser and add a new network using the address of your mining node:

### Help Your Mining Node Find the Network

When your mining node starts, it will only know about other miners listed in the genesis block. Depending on how long the network's been running, there's a good chance those original miners have good offline or changed their URLs. If that is the case, you need to help your mining node find other miners on the network. You can do this by sending a control transaction contaiing the URL of at least one active mining node:


Once you've done this, give your node a few minutes to find the rest of the network and catch up.

### Upgrade Your Account

Once your node is connected to the network, you are ready to upgrade your account and start mining.

At the time of this writing, self-serve mining accounts aren't supported. Those will entail obtaining a verified digital identity from a third party.

To upgrade your account, you'll need to ask someone with an account that has permission to register new miners. Right now, this will be a Cryptogogue team member. Contact whoever invited you to become a miner for information.
