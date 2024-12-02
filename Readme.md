# how-do-i-get-your-attention.com Kernel Networking Module

This project is a high-performance Linux kernel networking module developed for advanced packet inspection, classification, and routing. It is designed to handle **IPv4**, **IPv6**, **Explicit Congestion Notification (ECN)**, and packet fragmentation, while adhering to modern RFC standards such as **RFC 3168**, **RFC 8200**, and **RFC 9435**.

The module filters packets based on unicast and globally unique MAC addresses, ensuring compliance with differentiated services standards. It processes Ethernet frames, extracts and evaluates key protocol details, and logs information in real-time, including precise timing in nanoseconds. The focus is on robust performance and high-speed processing directly within the kernel, avoiding unnecessary user-space operations.

To install the module, clone the repository, build the kernel object file, and load it using the `insmod` command. Once loaded, the module begins processing network packets automatically. Logs can be viewed using `dmesg`, where detailed packet information and flags are printed for each captured frame. Removing the module can be done with the `rmmod` command when no longer required.

An example output might include details such as the flags and ECN state of the packets being processed. For instance, you might see whether a packet is marked as "Don't Fragment," "More Fragments," or "Last Fragment," or observe the ECN states like "ECT(1)" or "CE" for IPv4 and IPv6 traffic.

The module prioritizes safe memory management and efficient packet buffer handling, making it a reliable component for advanced networking tasks. Contributions to improve the module or adapt it for additional networking scenarios are welcome, provided they align with Linux kernel development standards.

This project is distributed under the GPLv2 license, ensuring its availability for open-source development and collaboration.

