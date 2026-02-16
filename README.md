# CollabWrite-C
CollabWrite-C: Real-Time Collaborative Editor in C, aims to build a lightweight
collaborative text editor that allows multiple users to edit a shared document in real time over a LAN.
Unlike existing large-scale solutions such as Google Docs, this project focuses on implementing the
core mechanisms—text editing structures, networking, and synchronisation—completely in C for
educational and experimental use. The editor uses efficient data structures (linked lists, stacks, buffers, tries)
to handle text modifications, provides undo/redo capabilities, and ensures consistency through
concurrency control and simplified Operational Transformation (OT). By integrating socket
programming, POSIX threads, and synchronisation primitives, the system supports multiple users
editing simultaneously while preserving correctness. The project blends theoretical data structure
concepts with practical system-level programming to create a functional collaborative tool that
demonstrates how real-time editors work under the hood.