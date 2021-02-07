# VOLWAL Miner Control Setup Guide

If you're using VOLWAL as your wallet, open it in your browser and add a new network using the address of your mining node. Once the network is online, you can import your account.

There is a secret 'admin' screen accessible from the network screen in VOLWOL. To access it, navigate to the network screen (the screen that shows the list of accounts on the network) and append "/admin" to the URL:

![Network Admin URL](images/volwal-network-admin-url.png)

After you hit enter, you should see a screen that looks like this:

![Network Admin Screen](images/volwal-network-admin-screen.png)

Here you can import the private key you generated earlier for miner control. To get the private key:

```
more ./ops/volume-volition/keys/control.priv.pem
```

That should display a key that looks like this:

```
-----BEGIN PRIVATE KEY-----
MGQCAQAwEAYHKoZIzj0CAQYFK4EEAAoETTBLAgEBBCCUdJh2JcRcBCdAlwDMR31y
gvyQ8RGzTHhPLCmEt/yzLqEkAyIAA1KdK3KMKIHZtrovu1FoXFXiwYYdeuZi+C8O
N75ij65Q
-----END PRIVATE KEY-----
```

Copy and paste the key into VOLWAL:

![Import Control Key](images/volwal-import-control-key.png)

Once you've imported the key, it will show up on the miner admin screen:

![Network Admin Screen With Key](images/volwal-network-admin-screen-with-key.png)

You will now also be able to access the network admin screen from the network dropdown menu:

![Network Admin Dropdown](images/volwal-network-admin-dropdown.png)

### Help Your Mining Node Find the Network

When your mining node starts, it will only know about other miners listed in the genesis block. Depending on how long the network's been running, there's a good chance those original miners have retired or changed their URLs. If that is the case, you will need to help your mining node find other active nodes. You can do this by sending a control command containing the URL of at least one active node:

![Extend Network Command](images/volwal-extend-network-command.png)

Once you've done this, give your node a few minutes to find the rest of the network and catch up.

(As of this writing the Extend Network modal doesn't have a network checker button like the Add Network modal has. We'll get around to that eventually. Sorry.)

