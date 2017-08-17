# p2p_communication-demo

## Intro

p2p_communication-demo is a demo tool for study or research on peer to peer 
communication mechanism through NAT. which is a lightweight tool easy to 
distribute.

It is part of the p2p_communication-libev, this version 'demo' was implemented
using mid-man transfer, which was the easiest but most robust method.

Further, I will try to implement steady version p2p_communication-libev, which 
will update version of p2p_communication-demo.

Current version: 1.0.0 | [G++](http://www.cprogramming.com/g++.html)

## Features

p2p_communication-demo is written in C++ and was only tried on Linux-Like 
platform. It's designed to be a lightweight implementation of chat mechanism
through Internet, in order to keep the resource usage as low as possible.

## Prerequisites

You should distributed it on Linux-like enviroment, and C++ compile like g++
must support C++11, C++0x is not enough.

### Get the latest source code

```bash
git clone https://github.com/xiangp126/p2p_communication-demo
cd p2p_communication-demo
make

on server:
./p2pserver
on client:
./p2pclient [SERVER_IP] [SERVER_PORT]
```

### Build and install with recent libsodium

You have to install libsodium 1.0.8 or later before building. See [Directly build and install on UNIX-like system](#linux).

## Installation

### Pre-build configure guide

For a complete list of available configure-time option,
try `configure --help`.

### Debian & Ubuntu


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


notes:

    ss-redir provides a transparent proxy function and only works on the
    Linux platform with iptables.

```

## Advanced usage

The latest shadowsocks-libev has provided a *redir* mode. You can configure your Linux-based box or router to proxy all TCP traffic transparently.

    # Create new chain
    root@Wrt:~# iptables -t nat -N SHADOWSOCKS

## Setup P2P_Communication-Demo

It's quite easy to use this demo, it did not waste you much time to distribute.

### Setup your server

```bash
on server:
./p2pserver
```

### Setup your client

```bash
on client:
./p2pclient [SERVER_IP] [SERVER_PORT]
```

