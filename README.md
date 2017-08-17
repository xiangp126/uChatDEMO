# p2p_communication-demo

## Intro

p2p_communication-demo is a demo tool for study or research on peer to peer communication mechanism through NAT. which is a lightweight tool easy to distribute.

It is part of the p2p_communication-libev, this version 'demo' was implemented
using mid-man transfer, which was the easiest but most robust method.

Further, I will try to implement steady version p2p_communication-libev, which 
will update version of p2p_communication-demo.

Current version: 1.0.0 | [G++](http://www.cprogramming.com/g++.html)

Travis CI: [![Travis CI](https://travis-ci.org/shadowsocks/shadowsocks-libev.svg?branch=master)](https://travis-ci.org/shadowsocks/shadowsocks-libev)

## Features

p2p_communication-demo is written in C++ and dependes on [libev](http://software.schmorp.de/pkg/libev.html). It's designed to be a lightweight implementation of chat mechanism through Internet, in order to keep the resource usage as low as possible.

## Prerequisites

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
    root@Wrt:~# iptables -t mangle -N SHADOWSOCKS
    root@Wrt:~# iptables -t mangle -N SHADOWSOCKS_MARK

    # Ignore your shadowsocks server's addresses
    # It's very IMPORTANT, just be careful.
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 123.123.123.123 -j RETURN

    # Ignore LANs and any other addresses you'd like to bypass the proxy
    # See Wikipedia and RFC5735 for full list of reserved networks.
    # See ashi009/bestroutetb for a highly optimized CHN route list.
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 0.0.0.0/8 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 10.0.0.0/8 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 127.0.0.0/8 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 169.254.0.0/16 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 172.16.0.0/12 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 192.168.0.0/16 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 224.0.0.0/4 -j RETURN
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -d 240.0.0.0/4 -j RETURN

    # Anything else should be redirected to shadowsocks's local port
    root@Wrt:~# iptables -t nat -A SHADOWSOCKS -p tcp -j REDIRECT --to-ports 12345

    # Add any UDP rules
    root@Wrt:~# ip route add local default dev lo table 100
    root@Wrt:~# ip rule add fwmark 1 lookup 100
    root@Wrt:~# iptables -t mangle -A SHADOWSOCKS -p udp --dport 53 -j TPROXY --on-port 12345 --tproxy-mark 0x01/0x01
    root@Wrt:~# iptables -t mangle -A SHADOWSOCKS_MARK -p udp --dport 53 -j MARK --set-mark 1

    # Apply the rules
    root@Wrt:~# iptables -t nat -A OUTPUT -p tcp -j SHADOWSOCKS
    root@Wrt:~# iptables -t mangle -A PREROUTING -j SHADOWSOCKS
    root@Wrt:~# iptables -t mangle -A OUTPUT -j SHADOWSOCKS_MARK

    # Start the shadowsocks-redir
    root@Wrt:~# ss-redir -u -c /etc/config/shadowsocks.json -f /var/run/shadowsocks.pid

## Shadowsocks over KCP

It's quite easy to use shadowsocks and [KCP](https://github.com/skywind3000/kcp) together with [kcptun](https://github.com/xtaci/kcptun).

The goal of shadowsocks over KCP is to provide a fully configurable, UDP based protocol to improve poor connections, e.g. a high packet loss 3G network.

### Setup your server

```bash
server_linux_amd64 -l :21 -t 127.0.0.1:443 --crypt none --mtu 1200 --nocomp --mode normal --dscp 46 &
ss-server -s 0.0.0.0 -p 443 -k passwd -m chacha20 -u
```

### Setup your client

```bash
client_linux_amd64 -l 127.0.0.1:1090 -r <server_ip>:21 --crypt none --mtu 1200 --nocomp --mode normal --dscp 46 &
ss-local -s 127.0.0.1 -p 1090 -k passwd -m chacha20 -l 1080 -b 0.0.0.0 &
ss-local -s <server_ip> -p 443 -k passwd -m chacha20 -l 1080 -U -b 0.0.0.0
```

