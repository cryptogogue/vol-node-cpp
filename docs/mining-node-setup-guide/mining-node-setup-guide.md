# Mining Node Setup Guide

This guide assumes you are running Ubuntu or a similar flavor of Linux.

Before you begin, make sure you have an active account on whatever Volition network you plan to join and have the genesis block for that network. When you start mining, you will need someone to upgrade your regular account to a mining account.

### Prepare Your Server

```
sudo apt update
sudo apt install curl docker docker-compose git openssl vim
```

Also, set up an SSH key for use with [github](https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent).

If you are setting up on Ubuntu, you may need to create an \~/.ssh/config file:

```
Host *
  AddKeysToAgent yes
  IdentityFile ~/.ssh/<ssh_key_filename>
```

Finally, make sure your server's port 80 is open. Open port 443 as well if you have a domain name and plan to configure HTTPS/SSL.

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

This will take a long time. You may see some red warning messages; they are not an immediate cause for concern:

![Harmless Warnings](images/build-warnings.png)

Once the build is done, you should see a message indicating that image was sucsessfully tagged.

![Build Success](images/build-success.png)

You can get a list of Docker images on your system by typing:

```
docker image ls
```

![Docker Images](images/docker-images.png)

If you look inside build.sh, you'll see it generates a header file containing some information from git, then it invokes docker to build the image.

The .env file has some settings you don't probably need to change. It is ignored by git; you may want to adjust it later in certain scenarios.

### Set Up the Docker Volume

The example docker-compose files map ./ops/volume-volition onto /var/lib/volition/ inside the Docker container. This folder should be configured to contain the genesis block, the volition.ini file and the mining keys.

Copy and rename the example volition.ini:

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

Generate the mining keys with the helper script:

```
./ops/make-keys.sh
```

The keys will be placed ./ops/volume-volition/keys. After running the script, you should have two sets of .pem files: one for mining and one for mining node control.

![Keys](images/mining-keys.png)

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

You can also verify the node is running by opening your web browser and navigating to your server's IP address or domain. You should see a .json dump of information about the node.

![Node JSON](images/node-json-by-ip.png)

The example volition.ini configures the node to write its log to ./ops/volume-volition/log. A new log is written each time the node is run.

With the example configuration, the node's state is written to ./ops/volume-volition/persist-chain as two sqlite3 databases. The filenames are prefixed with the hash of the genesis block.

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

If you don't have a *valid* email in your configuration, certbot will not work and you will be sad.

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

Open the conf file and edit it to include your domain in the SSL path. The domain must be formatted correctly for certbot to work:

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

### Upgrade Your Account

Once your node is connected to the network, you are ready to upgrade your account and start mining.

At the time of this writing, self-serve mining accounts aren't supported. Those will entail obtaining a verified digital identity from a third party.

To upgrade your account, ask someone with an administrator account to help you. You will need to send them the URL of your mining node.

If you don't need to send miner control commands, you are done and can take a break.

### Updating the Node

From time to time you will want to update to the latest mining ndoe software. To do so, SSH in to your server, pull the latest, rebuild and restart:

```
cd vol-node-cpp
git pull
./ops/build.sh
./ops/compose-volition-nginx-certbot/down.sh
./ops/compose-volition-nginx-certbot/up.sh
```

If you're not using nginx/cerbot:

```
./ops/compose-volition/down.sh
./ops/compose-volition/up.sh
```
