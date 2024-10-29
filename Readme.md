# We-Make-Software

## Purpose

Establishing clear ground rules for development is essential, especially in open-source environments, as it empowers everyone to build high-quality software. By setting up a robust development framework with `extern`, `EXPORT_SYMBOL()`, and well-structured header files, we’re creating a developer toolkit. This toolkit allows for modular, reusable components across kernel modules, setting the foundation for efficient, scalable development workflows.

Having access to files within our own project brings significant power to skilled developers who understand the strength of this approach. This level of insight often requires a deep understanding of modular design and kernel development, which goes beyond amateur skill levels.

This software is intended to encourage everyone to explore Linux and its potential in modular software development.

## Professional and Personal Insights

Consider the cost of employing a university-educated developer. I earn 170 DKK/hour before tax, without pension benefits. This wage is fair, as I bring considerable value to the company. Working in IT, I can independently create and develop tools, though sometimes it’s unclear what others in the team can contribute.

However, if I build this project as an open-source initiative, I would consider collaborating with the community by hosting it on GitHub or another platform. This approach could attract sponsorship from developers, organizations, or companies that support open-source innovation.

Investing in this project enables me to provide valuable software solutions that benefit both my company and the broader community. For businesses engaging in e-commerce, promotions, or social media, I plan to integrate tools that simplify these tasks. The code will start with `WeMakeSoftware.c`, as the company will be branded as "We-Make-Software." This company aims to be an online-based provider of comprehensive software solutions.

## Vision and Goals

I intend to build software that helps companies identify and correct system issues. This project will evolve into a full cloud-based system, covering storage, websites, reviews, social media integration, and everything necessary to promote and manage a business online.

Think of a **Makefile** as a developer tool that can help enhance programming skills. With `WeMakeSoftware.h`, we’re focusing on software development specifically—since hardware is already being innovated upon by high-tech companies. The name `WeMakeSoftware.h` reflects this focus.

## Why Linux and C?

Many rely on Linux for tasks ranging from web servers to VoIP. C remains the closest language to the Linux kernel, making it the most powerful option for system-level development. I use Ubuntu due to its advanced features, but C is integral to building kernel-related solutions.

## Networking and Redundancy

For disk management, Linux offers flexibility. You can create disks bound to multiple physical drives with or without RAID. I believe RAID is essential for redundancy and reliability. Proper system redundancy ensures that data is safe even if a drive fails.

Building the first server correctly is a source of accomplishment—it’s a testament to doing things right from the start.

## Networking Layer Details

The Data Link Layer Frame Structure contains the following components:

- **Destination MAC Address**: 6 bytes
- **Source MAC Address**: 6 bytes
- **EtherType**: 2 bytes
- **Payload**: Up to 1500 bytes
- **Frame Check Sequence (FCS)**: 4 bytes

The FCS, computed by the network device, is used to detect transmission errors. When a frame is sent, the sender calculates the FCS based on the frame contents. This error-detecting code, crucial at the Data Link Layer (Layer 2) of the OSI model, ensures data integrity during transmission. Since the FCS is processed by the network interface card (NIC) before reaching the kernel, it’s outside our direct control. Therefore, our focus remains on processing the relevant headers and payloads in the kernel.

### Local and Non-Local Packets

If the Destination MAC Address and Source MAC Address suggest the packet is local, it indicates the packet is handled within the server. Since we’re focused on non-local packets, a check is implemented to verify if these addresses are zero or non-zero.

### Ethernet Frame Representation

| Field                   | Binary Representation                                              |
|-------------------------|--------------------------------------------------------------------|
| Destination MAC Address | 10101000 10100001 01011001 11000000 01010110 00100100             |
| Source MAC Address      | 00000000 01010000 01010110 10101000 10100010 11001000             |
| EtherType               | 00001000 00000000                                                 |


EtherType helps identify the protocol encapsulated in the payload of an Ethernet frame. A full list of EtherTypes is publicly available on the IANA website: [IEEE 802 Numbers](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml). The first column displays the decimal value, which indicates the specific EtherType in use.


Since we are focusing exclusively on public network protocols, we only need to consider [**IPv4 (2048)**](https://www.rfc-editor.org/rfc/rfc791) and [**IPv6 (34525)**](https://www.rfc-editor.org/rfc/rfc8200.html). Although we refer to it as **IPAddressType**, this field is effectively the same as the **EtherType**.


