# We-Make-Software

## Purpose

Establishing clear ground rules for development is essential, especially in open-source environments, as it enables developers to build high-quality software. By setting up a robust development framework with `extern`, `EXPORT_SYMBOL()`, and well-structured header files, we’re creating a toolkit of modular, reusable components across kernel modules, setting the foundation for efficient and scalable development workflows.

Having access to project-specific files empowers skilled developers who understand the value of modular design and kernel development. This level of insight requires a deep understanding of these principles, which go beyond amateur skill levels.

This project encourages exploration of Linux's potential in modular software development.

## Professional and Personal Insights

Consider the cost of employing a university-educated developer. I earn 170 DKK/hour before tax, without pension benefits. This wage is fair, as I bring considerable value to the company. Working in IT, I can independently create and develop tools, though sometimes it’s unclear what others in the team can contribute.

If I build this project as an open-source initiative, I would consider hosting it on GitHub or another platform to attract sponsorship from developers, organizations, or companies supporting open-source innovation.

Investing in this project enables me to provide valuable software solutions that benefit both my company and the broader community. For businesses engaging in e-commerce, promotions, or social media, I plan to integrate tools to simplify these tasks. The code will start with `WeMakeSoftware.c`, aligning with the company's brand as "We-Make-Software," an online provider of comprehensive software solutions.

## Vision and Goals

I intend to build software that helps companies identify and correct system issues. This project will evolve into a full cloud-based system covering storage, websites, reviews, social media integration, and everything necessary to promote and manage a business online.

A **Makefile** is a powerful developer tool that can help enhance programming skills. With `WeMakeSoftware.h`, we’re focusing on software development, as hardware innovation is already driven by high-tech companies. The name `WeMakeSoftware.h` reflects this focus.

## Why Linux and C?

Many rely on Linux for tasks ranging from web servers to VoIP. C, as the language closest to the Linux kernel, is the most powerful option for system-level development. I use Ubuntu for its advanced features, but C remains essential for building kernel-related solutions.

## Networking and Redundancy

Linux offers flexibility for disk management, allowing for drives bound to multiple physical disks, with or without RAID. RAID provides redundancy and reliability, ensuring data safety even if a drive fails.

Building the first server correctly is a testament to doing things right from the start.

## Networking Layer Details

The Data Link Layer Frame Structure contains:

- **Destination MAC Address**: 6 bytes
- **Source MAC Address**: 6 bytes
- **EtherType**: 2 bytes
- **Payload**: Up to 1500 bytes
- **Frame Check Sequence (FCS)**: 4 bytes

The FCS, calculated by the network device, detects transmission errors. During transmission, the sender calculates the FCS based on the frame's contents. This error-detecting code, crucial at the Data Link Layer (Layer 2) of the OSI model, ensures data integrity during transmission. Since the FCS is processed by the NIC before reaching the kernel, we focus on processing the relevant headers and payloads in the kernel.

### Local and Non-Local Packets

If the Destination and Source MAC Addresses indicate a local packet, it’s handled within the server. Since we’re focusing on non-local packets, a check verifies if these addresses are zero or non-zero.

### Ethernet Frame Representation

| Field                   | Binary Representation                                              |
|-------------------------|--------------------------------------------------------------------|
| Destination MAC Address | 10101000 10100001 01011001 11000000 01010110 00100100             |
| Source MAC Address      | 00000000 01010000 01010110 10101000 10100010 11001000             |
| EtherType               | 00001000 00000000                                                 |

EtherType identifies the protocol in the payload of an Ethernet frame. A complete EtherType list is available on the IANA website: [IEEE 802 Numbers](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml). The first column shows the decimal value indicating the specific EtherType in use.

We focus on public network protocols, specifically [**IPv4 (2048)**](https://www.rfc-editor.org/rfc/rfc791) and [**IPv6 (34525)**](https://www.rfc-editor.org/rfc/rfc8200.html). This field matches the **EtherType**.

### IPv4 (2048) & IPv6 (34525) First Bytes from Payload

| Field                     | IPv4       | IPv6                  |
|---------------------------|------------|-----------------------|
| Version                   | ✅ 0100    | ✅ 0110               |
| IHL (Internet Header Length) | ✅ 4 bits | ❌                    |
| Type of Service / Traffic Class | ✅ 8 bits | ✅ 8 bits         |
| Flow Label                | ❌         | ✅ 20 bits            |
| Total Length / Payload Length | ✅ 16 bits | ✅ 16 bits       |
| TTL / Hop Limit           | ✅ 8 bits  | ✅ 8 bits            |
| Protocol / Next Header    | ✅ 8 bits  | ✅ 8 bits            |
| Header Checksum           | ✅ 16 bits | ❌                    |
| Source Address            | ✅ 32 bits | ✅ 128 bits          |
| Destination Address       | ✅ 32 bits | ✅ 128 bits          |
| Options / Extension Headers | ✅ Optional | ✅ Extension Headers |

Both IPv4 and IPv6 have a version field: for IPv4, it is 4, and for IPv6, it is 6.

In IPv4, the IHL (Internet Header Length) has a minimum value of 5 and a maximum of 15. This value determines the start of the data following the IPv4 (2048) header.

In IPv4 and IPv6 headers, specific fields serve **Quality of Service (QoS)** and **traffic prioritization** purposes. Although named differently—**Type of Service** in IPv4 and **Traffic Class** in IPv6—they serve similar functions:

- **Packet Prioritization**: Both fields allow network devices to prioritize packets, managing traffic flow for critical or time-sensitive data, such as VoIP or video streaming.
- **Congestion Management**: These fields support **Explicit Congestion Notification (ECN)**, signaling network congestion to dynamically adjust traffic flow and prevent packet loss.

**Type of Service** in IPv4 aligns with **Differentiated Services (DiffServ)** standards, while **Traffic Class** in IPv6 was designed with DiffServ in mind, providing a streamlined implementation. Both fields aid in packet prioritization and congestion management, ensuring efficient data flow across the network.