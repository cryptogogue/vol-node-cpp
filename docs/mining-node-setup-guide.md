# Mining Node Setup Guide

Volition is meant to run on a dedicated server or a reasonably-powered VPS. The current implementation is  gated more by bandiwdth and storage than CPU or RAM, though long-term performance with larger mining networks remains to be seen.

This guide assumes you are running Ubuntu or a similar flavor of Linux. This is not (yet) an exhaustive guide; you may still need to search and troubleshoot.

Also, before you begin, make sure you have an active account on whatever Volition network you plan to join and have the genesis block for that network. When you start mining, you will need someone to upgrade your regular account to a mining account. You may also want to provision a separate account just for mining.

### Prepare Your Server

```
sudo apt update
sudo apt install curl docker docker-compose git openssl vim
```

Also set up an SSH key for use with github:

https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent

Check your server's settings and make sure port 80 is open. Open port 443 if you plan to configure HTTPS/SSL.

### Clone and Build the Node

```
git clone --recurse-submodules git@github.com:cryptogogue/vol-node-cpp.git
```

Build the docker image using a helper script:

```
cd vol-node-cpp
cp ./ops/.env.example ./ops/.env
./ops/build.sh
```

This will take a long time. You may see some red warning messages; they are not immediate cause for concern.

Once the build is done, you should see a message indicating that image was sucsessfully tagged.

You can get a list of Docker images on your system by typing:

```
docker image ls
```

If you look inside build.sh, you'll see it generates a header file containing some information from git, then it invokes docker to build the image.

The .env file has some settings you don't probably need to change. It is ignored by git; you may want to adjust it later in certain advanced scenarios (if you need to deploy multiple versions of the node on the same machine, for example).

### Set Up the Docker Volume

The example docker-compose files map ./ops/volume-volition onto /var/lib/volition/ inside the Docker container. This folder should be configured to contain the genesis block, the volition.ini file and the mining keys.

Copy the example volition.ini:

```
cp ./ops/volume-volition/volition.ini.example ./ops/volume-volition/volition.ini
```

Edit volition.ini by adding the name of the account you plan to use for mining:

```
control-key                 = /var/lib/volition/keys/control.pub.pem
control-level               = config
genesis                     = /var/lib/volition/genesis.json
keyfile                     = /var/lib/volition/keys/mining.priv.pem
logpath                     = /var/lib/volition/log
openSSL.client.caConfig     = /etc/ssl/certs
persist                     = /var/lib/volition/persist-chain
port                        = 9090

miner                       = <your miner account name>
```

You can generate the keys using the helper script:

```
./ops/make-keys.sh
```

The keys will be placed ./ops/volume-volition/keys. After running the script, you should have two sets of pem files there: one for mining and one for mining node control.

Finally, use curl to fetch the genesis block:

```
curl <URL of genesis block> -o ./ops/volume-volition/genesis.json
```

### Run the Mining Node

```
./ops/compose-volition/up.sh
```

This will run the mining node. You can verify the node is running by typing:

```
docker ps
```

This will list the currently running containers.

You can also verify the node is running by opening your web browser and navigate to your server's IP address or domain. You should see a .json dump of information about the node.

The example volition.ini configured the node to write its log to ./ops/volume-volition/log. A new log is written each time the node is run.

With the example configuration, the node's state is written to ./ops/volume-volition/persis-chain as two sqlite3 databases. The filenames are prefixed with the has of the genesis block. If you are curious, you can download them and use a sqlite client to check their contents.

When you are ready to stop the node, you can use the helper script:

```
./ops/compose-volition/down.sh
```

### Configuring HTTPS/SSL

To use HTTPS/SSL you will need a domain name. Procuring a domain name is outside of the scope of this document; check with your hosting provider for instructions.

First, make sure there is a *valid* email address in ./ops/.env:

```
CERTBOT_EMAIL=totally_valid_email@some_real_domain.com
```

If you don't have a *valid* email in your configuration, certbot will not work.

Copy the example nginx configuration directory in volume-nginx:

```
cp -r ./ops/volume-nginx.example ./ops/volume-nginx
```

Now rename the example configuration file to your domain:

```
mv ./ops/volume-nginx/user.conf.d/www.mydomain.com.conf ./ops/volume-nginx/user.conf.d/<mysubdomain>.<mydomain>.<mydomainextension>.conf
```

For example, if you own the domain 'volhodler.net' and you want to access the node at the subdomain 'node':

```
mv ./ops/volume-nginx/user.conf.d/www.mydomain.com.conf ./ops/volume-nginx/user.conf.d/node.volhodler.net.conf
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

Now you can run volition with nginx and certbot:

```
./ops/compose-volition-nginx-certbot/up.sh
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
