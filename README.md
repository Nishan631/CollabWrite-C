**CollabWrite-C**

Real-Time Collaborative Text Editor in C (LAN-Based)

CollabWrite-C is a lightweight real-time collaborative text editor built entirely in C.
It demonstrates how modern collaborative systems (like Google Docs) work internally by implementing core mechanisms such as text data structures, networking, concurrency control, and simplified Operational Transformation (OT).

This project focuses on systems-level programming, distributed synchronization, and efficient data structure design, making it suitable for educational, experimental, and research purposes.

 **Features**

1.Multi-user real-time editing over TCP (LAN)

2.Simplified Operational Transformation (OT) for consistency

3.Multi-threaded server using POSIX threads

4.Socket-based client-server architecture

5.Custom text buffer implementation (linked list/buffer-based)

6.Undo/Redo support using stack structures

7.Trie-based autocomplete system

8.Version tracking for edit synchronization

9.Modular architecture with separation of concerns

 **Architecture Overview**

CollabWrite-C follows a layered architecture:

Application Layer
 ├── client.c
 └── server.c

Networking Layer
 └── network.c / network.h

Collaboration Logic
 ├── editoperation.c / editoperation.h
 └── version.c / version.h

Data Structure Layer
 ├── text_buffer.c / text_buffer.h
 └── trie_suggest.c / trie_suggest.h

**Execution Flow**

Server initializes socket and listens for connections.

Clients connect over TCP.

Each client sends edit operations (insert/delete).

Server:

Applies transformation logic

Updates shared document state

Broadcasts changes to all connected clients

Clients update their local buffers accordingly.

**Core Concepts Implemented**

1️⃣ Operational Transformation (OT)

Ensures consistency when multiple users edit the document concurrently by transforming operations relative to version history.

2️⃣ Version Management

Tracks document revisions to maintain ordering and prevent conflicts.

3️⃣ Custom Text Buffer

Efficient text handling using linked list/buffer structures instead of naive string manipulation.

4️⃣ Trie-Based Autocomplete

Implements prefix-based word suggestion using a trie data structure.

5️⃣ Multi-threading

Server handles multiple clients concurrently using POSIX threads and synchronization primitives.

**Technologies Used**

C (C99)

POSIX Sockets

POSIX Threads (pthread)

TCP/IP Networking

Custom Data Structures (Linked Lists, Stacks, Tries)

Makefile Build System

📂 **Project Structure**
collabwrite-c/
│
├── Makefile
├── src/
│   ├── client.c
│   ├── server.c
│   ├── network.c
│   ├── text_buffer.c
│   ├── editoperation.c
│   ├── version.c
│   ├── trie_suggest.c
│   └── words.txt
└── .vscode/

**Build Instructions (Linux/macOS)**

1️⃣ Clone Repository
git clone <repository_url>
cd collabwrite-c

2️⃣ Build
make


This generates:

server
client

3️⃣ Run Server
./server

4️⃣ Run Client (in another terminal)
./client

To simulate collaboration, run multiple client instances.

**Windows Support**

To run on Windows:

Install MinGW-w64

Replace POSIX socket calls with Winsock equivalents

Link with -lws2_32

Initialize Winsock using WSAStartup()

Future improvements include cross-platform abstraction for sockets and threading.

**Example Use Case**

Multiple users on the same LAN connect to the server.

Users type simultaneously.

Server resolves concurrency using OT.

All clients remain consistent in real time.

**Future Improvements**

Vector clocks for causal consistency

CRDT-based synchronization

Non-blocking I/O using select() or epoll()

Persistent document storage and recovery

ncurses-based terminal UI

Performance benchmarking under concurrent load

Cross-platform abstraction layer (Linux + Windows)


This project bridges theoretical computer science concepts (DSA, concurrency, distributed systems) with practical systems programming.
It serves as a hands-on implementation of how collaborative editors maintain consistency under concurrent edits.

Author:
Nishandeep Singh
B.Tech Computer Science
