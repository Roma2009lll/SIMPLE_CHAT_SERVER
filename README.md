# TCP Chat Server (Sequential)

Simple TCP chat server in C with Winsock.

## Features
- Sequential client handling
- Commands: /nick, /msg, /history, /quit
- Message history (100 messages)

## Build
```
cl server.cpp ws2_32.lib
cl client.cpp ws2_32.lib
```

## Run
Terminal 1: `server.exe`
Terminal 2: `client.exe`

## Commands
- `/nick <name>` - Set nickname
- `/msg <text>` - Send message
- `/history` - View history
- `/quit` - Disconnect
