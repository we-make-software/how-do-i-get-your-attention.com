If we establish clear ground rules for development, it will allow everyone to create the best software possible, especially in open-source environments.

By setting up a solid structure using extern, EXPORT_SYMBOL(), and properly organizing header files, you're essentially building a developer toolkit. This toolkit enables the creation of modular, reusable components that can be shared across kernel modules and easily reused in future projects. This is how you're laying the foundation for an efficient development workflow.

Having access to files within our own project is incredibly powerful. Skilled developers will understand the strength of this approach. However, this level of insight goes beyond what an amateur developer might grasp. It requires deep knowledge of modular design and kernel development, and I want you to consider that.

This software is meant to encourage everyone to use Linux. It's foundational in that regard.

Now, think about this: how much do you pay to have a university-educated professional in your company? Where I work, I earn 170 Danish kroner before tax, without pension benefits. It’s fair, because I can significantly improve the company.

I work in the IT section, and although I can develop things myself at my own pace, it’s not always clear what others can do. But if I build an open-source project, I would consider collaborating with the community. I could host the project on GitHub or another platform, and possibly gain sponsorship from developers, organizations, or companies that support open-source initiatives.

I invest in myself so I can provide the best software solutions for companies. What I can create for everyone, where I work, can be beneficial to others too. For anyone looking to sell products online, run promotions, or engage with social media, I can integrate these solutions into my project, providing clear and concise descriptions in the code. The code will begin with WeMakeSoftware.c, as the company will be named "We-Make-Software." I plan to elevate "We-Make-Software" as an online-based company.

What I aim to develop is software that helps companies correct mistakes in their systems. I’m planning to build a full cloud-based system—covering storage, websites, reviews, social media, and everything needed to promote a company.


If you think about it, a Makefile is a tool that can help developers become more advanced and improve their skills. It's the Makefile.

Well, we can set a standard. By creating WeMakeSoftware.h, we're focusing on software, not hardware. It's called WeMakeSoftware.h because we're not making hardware, which is already developed or being upgraded by high-tech companies.

Today, many people use Linux for everything from web servers to VoIP. But when it comes to programming languages, C is the closest to the Linux kernel. I use Ubuntu because it's pretty "high tech," but C is the most powerful language close to the kernel.

Why make something bigger when you can create it yourself? I'm not sure, but we can try. Okay, the Linux file system is pretty good. We can create a disk that is bound to multiple disks, depending on whether you need RAID or not. 

I believe we need RAID. Why? Because if one disk fails, the other disk still has the data. This reminds me that we need to ensure redundancy and reliability in our systems.

When we build something correctly, like the first server, it gives a sense of accomplishment.

Have you ever had that feeling where you seem to be doing the same code or following the same routine forever? I have a great explanation for network data.


If wee look indside the Data Link Layer Frame Structures

We follow .:

Destination MAC Address (6 bytes)
Source MAC Address (6 bytes)
EtherType (2 bytes)
Payload (up to 1500 bytes)
Frame Check Sequence (4 bytes)

Frame Check Sequence also call FCS it made from the network device.  


In examining the Data Link Layer Frame Structures, we see the following components:

- **Destination MAC Address** (6 bytes)
- **Source MAC Address** (6 bytes)
- **EtherType** (2 bytes)
- **Payload** (up to 1500 bytes)
- **Frame Check Sequence (FCS)** (4 bytes)

The Frame Check Sequence (FCS) is computed by the network device and is used to detect errors that may have occurred during the transmission of data across a network. When a frame is transmitted, the sender calculates the FCS based on the contents of the frame. 

The FCS is an error-detecting code used in networking protocols, primarily at the Data Link Layer (Layer 2) of the OSI model, to ensure the integrity of transmitted data. While the FCS plays an important role in error detection, its handling is often outside our direct control in the kernel, as it is processed by the network interface card (NIC) before the data reaches our code.

Focusing on the FCS in our networking code may not be necessary since the kernel manages this aspect automatically. Our attention is better directed towards processing the relevant headers and payloads of the packets.


If the Destination MAC Address and Source MAC Address indicate that the packet is local, it means that the packet is being handled within the server itself. Since we focus on non-local packets, I have implemented a check to determine if these addresses are zero or not.



10101000 10100001 01011001 11000000 01010110 00100100 Destination MAC Address
00000000 01010000 01010110 10101000 10100010 11001000 Source MAC Address
00001000 00000000 EtherType



We can use EtherType to identify the protocol encapsulated in the payload of an Ethernet frame. A comprehensive list of EtherTypes can be found at the IANA website: [IEEE 802 Numbers](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml). This resource is publicly available and easy to understand.

In this list, the first column displays the decimal number, which indicates the specific EtherType we are working with.

