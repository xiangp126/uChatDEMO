## Design

- It is a tool implementation of peer to peer communication through NAT.
    - communication method : udp
    - principle : mid-man transfer

Current version: 1.2.0 | [G++](http://www.cprogramming.com/g++.html)

## Feature List

### V1.2.0
* fix bug on mac: ::bind() expliciyly
* Fix Minor: fix length of command 'list' output
* Add config.h for key parameters easily modify

### V1.1.0
* Fix Bug: Let Logout Delete Punched Info
* Add SET hostname Support, Make Peer More Identified

### V1.0.0
* Whoami Client Query Support
* RealTime Login and Punched Info Output
* KeepAlive Mechanism from Client to Server
* Dynamic Connection Establishment
* Directly peer to peer chat, without any redundant command word
* Pretty Print on Server Side Message

## Quick Start

### Prerequisites

- gcc must support c++ 11, only c++0x support not enough
- you should has one linux server with public ip

### Get the latest source code

```bash
git clone https://github.com/xiangp126/uchat
cd uchat
make
# p2pserver & p2pclient was generated
```

### Setup your server
    on your server
```bash
vim config.h
# change PORTNUM if needed
# and ensure the listening port was not blocked by your company or ISP

/* listen port on the server side */
#define PORTNUM    13000

make
./bin/p2pserver
Now listening on port 13000...

```

### Setup your client
    on your client
```bash
make
./bin/p2pclient [SERVER_IP] [LISTEN_PORT]
>>>
# type 'help'
>>> help
NAME
    p2pclient --- Client End of Peer to Peer communication Through NAT

USAGE
    >>> COMMAND [PARAM]
    type COMMANDS before any word you typed. If you want to type COMMANDS
    as a word itself, just leave a blank before it. Default you chat with
    the server, after punched, you chat directly with the peer you punched.

PARAM:
    help
    list
    whoami
    punch [ip] [port]
    login
    logout
    setname [hostname]
    exit

DESCRIPTION
    HELP:    print this help info, you can type 'help' when needed.
    LIST:    print logined and punched info. It's useful before 'punch'.
    WHOAMI:  show who you are, namely the ip address & port through NAT.
    PUNCH:   establish connection between you and the peer you want to talk with.
             After punched, you talked directly to punched peer.
             punch [ip] [port], such as: punch [64.0.1.5] [12400]
    LOGIN:   login you existence to the server, may type 'list' when logined.
    LOGOUT:  clear your login info on the server.
    SETNAME: set hostname to make peer more identified. When logined, setname will
             take effect at once. The hostname will be back to Annoymous after
             logout. Format: setname [hostname], suck as: setname corsair
             That is: setname only take effect after logined.
    EXIT:    exit this program on your machine, same as CTRL + D.

AUTHORS
       p2pclient is a DEMO for either further study or research purpose, you
       can modify or redistribute it whatever you want.
```

## EXAMPLE

    This example illustrates both the client and server side.

    On server side, the message reveived was pretty printed as default.

### Client Side

```bash
>>> list
>>> Message From (127.0.0.1 13000):
-------------------------- *** Login Info
  PEERINFO-IP-PORT       TTL   HOSTNAME
  127.0.0.1 35079        10    arthur     <= already logined
  127.0.0.1 47061        9     corsair    <= already logined
*** --------------------------------------

-------------------------- *** Punch Info
*** --------------------------------------

# arthur was another peer name set by >>> setname arthur
# default name is Annoymous
>>> punch arthur
Usage Format: punch 127.0.0.1 18974
Notice Not Leave any blank before 'punch'.
>>> whoami
>>> You Are: (64.104.169.98 58404)
>>> punch 127.0.0.1 35079
Want To Punch (127.0.0.1 35079)
Sending PUNCH Packet To Server...
>>> NOTICE: First, You Two Must All Be Logined.
Just Type 'list' to See Info.

>>> login
>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  PEERINFO-IP-PORT       TTL   HOSTNAME
  64.104.169.98 58404    10    Annoymous   <= it's me, newly logined
  127.0.0.1 35079        9     arthur
  127.0.0.1 47061        9     corsair
*** --------------------------------------

-------------------------- *** Punch Info
*** --------------------------------------

>>> setname giggle
>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  PEERINFO-IP-PORT       TTL   HOSTNAME
  64.104.169.98 58404    10    giggle     <= rename default 'Annoymous' to giggle
  127.0.0.1 35079        10    arthur
  127.0.0.1 47061        10    corsair
*** --------------------------------------

-------------------------- *** Punch Info
*** --------------------------------------

>>> punch 127.0.0.1 35079
Want To Punch (127.0.0.1 35079)
Sending PUNCH Packet To Server...
>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  PEERINFO-IP-PORT       TTL   HOSTNAME
  64.104.169.98 58404    9     giggle
  127.0.0.1 35079        9     arthur
  127.0.0.1 47061        10    corsair
*** --------------------------------------

-------------------------- *** Punch Info
  127.0.0.1 35079   ===>>   64.104.169.98 58404     <= newly punched info (one pair)
  64.104.169.98 58404   ===>>   127.0.0.1 35079
*** --------------------------------------

On My Side:
>>> can you speak English?
>>> I am LiLei.
>>> Message From (127.0.0.1 35079): My name is HanMeiMei.
>>>

Meanwhile On the Peer Side:
>>> Peer (64.104.169.98 58404) Want To Chat With You.
By Default, Auto Send Accept.

>>> Message From (64.104.169.98 58404): can you speak English?
>>> Message From (64.104.169.98 58404): I am LiLei.
>>> My name is HanMeiMei.
>>>

```

### Server Side

```bash

Head Type = MESSAGE
Payload length = 424 (Had + '\0')
Head PeerInfo = (0.0.0.0 0)
Packet Payload:
-------------------------- *** Login Info
  PEERINFO-IP-PORT       TTL   HOSTNAME
  64.104.169.98 58404    10    giggle
  127.0.0.1 35079        8    arthur
  127.0.0.1 47061        9    corsair
*** --------------------------------------

-------------------------- *** Punch Info
  127.0.0.1 35079   ===>>   64.104.169.98 58404
  64.104.169.98 58404   ===>>   127.0.0.1 35079
*** --------------------------------------

Head Type = SETNAME
Payload length = 15 (Had + '\0')
Head PeerInfo = (0.0.0.0 0)
Packet Payload: setname giggle

Heart Beat Received From (64.104.169.98 58404)
Head Type = HEARTBEAT
Payload length = 11 (Had + '\0')
Head PeerInfo = (0.0.0.0 0)
Packet Payload: HI Server.

```
