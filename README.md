# p2p_communication-demo

## Intro

p2p_communication-demo is a demo tool for study or research on peer to peer communication mechanism through NAT. which is a lightweight tool very easy to distribute.

It is part of the p2p_communication-libev, this version 'demo' was implemented
using mid-man transfer, which was the easiest but most robust method.

Further, I will try to implement steady version p2p_communication-libev, which was updated version of this p2p_communication-demo.

Current version: 1.0.0 | [G++](http://www.cprogramming.com/g++.html)

## Features

p2p_communication-demo is written in C++ and was only tested on Linux-Like platform. 
It's designed to be a lightweight implementation of chat mechanism through Internet, in order to keep the resource usage as low as possible.

## Usage

For a detailed and complete list of all supported arguments,
you may refer to the help pages of the applications, respectively.

```bash
>>> help
NAME
    p2pclient --- Client End of Peer to Peer communication Through NAT

USAGE
    >>> COMMAND [PARAM]
    type COMMANDS before any word you typed. If you want to type COMMANDS
    as a word itself, just leave a blank before it. Default you chat with
    the server, after punched, you chat directly with the peer you punched.

SYNOPSIS:
    help
    list
    whoami
    punch [ip] [port]
    login
    logout
    exit

DESCRIPTION
    HELP:   print this help info, you can type 'help' when needed.
    LIST:   print logined and punched info. It's useful before 'punch'.
    WHOAMI: show who you are, namely the ip address & port through NAT.
    PUNCH:  establish connection between you and the peer you want to talk with.
            After punched, you talked directly to punched peer.
            punch [ip] [port], such as: punch [64.0.1.5] [12400]
    LOGIN:  login you existence to the server, may type 'list' when logined.
    LOGOUT: clear your login info on the server.
    EXIT:   exit this program on your machine, same as CTRL + D.

AUTHORS
       p2pclient is a DEMO for either further study or research purpose, you
       can modify or redistribute it whatever you want.

```

## EXAMPLE

This example was both from the client and server point.
On server side, the message reveived was pretty printed as default. 

### Client Side

```bash
>>> login
>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  10.124.10.102 57569   ===>>   TTL: 9
*** ------------------------------

-------------------------- *** Punch Info
*** --------------------------------------

>>> punch
Usage Format: punch 127.0.0.1 18974
Notice Not Leave any blank before 'punch'

>>> punch 10.124.10.102 57569
Want To Punch (10.124.10.102 57569)
Sending PUNCH Packet To Server...
>>> NOTICE: First, You Two Must All Be Logined.
Just Type 'list' to See Info.
>>>

>>> whoami
>>> You Are: (10.124.10.102 46302)

>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  10.124.10.102 46302   ===>>   TTL: 10
  10.124.10.102 57569   ===>>   TTL: 10
*** ------------------------------

-------------------------- *** Punch Info
*** --------------------------------------

>>> punch 10.124.10.102 57569
Want To Punch (10.124.10.102 57569)
Sending PUNCH Packet To Server...
>>> list
>>> Message From (10.124.10.102 13000):
-------------------------- *** Login Info
  10.124.10.102 46302   ===>>   TTL: 10
  10.124.10.102 57569   ===>>   TTL: 10
*** ------------------------------

-------------------------- *** Punch Info
  10.124.10.102 57569   ===>>   10.124.10.102 46302
  10.124.10.102 46302   ===>>   10.124.10.102 57569
*** --------------------------------------

On the Peer Side:
>>> Peer (10.124.10.102 46302) Want To Chat With You.
By Default, Auto Send Accept.

>>> can you speak English?
>>> I am LiLei.
>>>

>>> Message From (10.124.10.102 57569): My name is HanMeiMei.
>>>

```

### Client Side
```bash

```

## Installation

### Prerequisites

You should distribute it on Linux-like enviroment, and C++ compile such as g++
must support C++11.
Only C++0x support may not be enough.

### Get the latest source code

```bash
git clone https://github.com/xiangp126/p2p_communication-demo
cd p2p_communication-demo
make
```

### Setup your server

```bash
on server:
./p2pserver [LISTEN_PORT]
# default LISTEN_PORT is 13000
Now listening on port 13000...
```

### Setup your client

```bash
on client:
./p2pclient [SERVER_IP] [SERVER_PORT]
>>> 
# type 'help' 
>>> help

```

