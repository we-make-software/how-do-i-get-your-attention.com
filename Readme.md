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

## Why Linux and C?

Many rely on Linux for tasks ranging from web servers to VoIP. C, as the language closest to the Linux kernel, is the most powerful option for system-level development. I use Ubuntu for its advanced features, but C remains essential for building kernel-related solutions.

## Networking and Redundancy

Linux offers flexibility for disk management, allowing for drives bound to multiple physical disks, with or without RAID. RAID provides redundancy and reliability, ensuring data safety even if a drive fails.

Building the first server correctly is a testament to doing things right from the start.


#### Further Information

Basic programming in IEEE 802.3 in C is a foundational element. We-Make-Software.com aims to build an open-source project for IEEE 802.3, beginning with our initial build version.

# This is what we see:

When data enters our network card, it follows the IEEE 802 standard, a foundational protocol in networking. Packets come in various types, identified by an Ethertype, which is critical for firewalls worldwide to manage and control network traffic.

We only accept data larger than 42 bytes.

Our goal is to monitor and support clients by providing tasks and services. Payments can be made directly to We-Make-Software, enabling us to assist clients effectively. This is our firewall, built to adhere to IPv4 standards as defined by RFC 791 and RFC 8200. Our solution is open source, and we publish it when we have the time and resources to do so.

We strive to be a valuable tool, and our design choices reflect this commitment.

Fewer functions mean faster execution, but we ensure a robust system by using pointers and structures to manage how data is read and processed.

When data arrives, we perform the default checks in the `FrameReader` function, as demonstrated in our implementation. The name "Frame" reflects the nature of what we process—a frame of data. Our goal is to understand and handle the frame efficiently.

If this is valid, I have created a structure called `Frame`:

```c
struct Standard {
    struct Standard*Previous,*Next;
    uint16_t Version,Section;
    char*Data;
};

struct Frame {
    struct Frame*Previous,*Next;
    struct sk_buff*skb;
    int id;
    char*IEE802Buffer;
    struct Standard*Standards;
};
```

As you can see, the `Frame` structure includes `IEE802Buffer` and `id`, which represent the buffer and the network card it originates from, respectively. I have also included `Previous` and `Next` pointers to make this `Frame` structure part of a linked list in memory. This list is managed by the global variable:

```c
static struct Frame*Frames=NULL;
```

This setup allows seamless management of frames, with `static struct Frame*CreateFrame(uint8_t id)` as the function responsible for creating and linking new frames. This function does not have a connection to the `sk_buff*skb`, so I have implemented `SetSizeFrame(struct Frame*frame,uint16_t Size)` to create a new `sk_buff` only if one is not already installed. This means that in the `FrameReader` function, we don’t need to use it immediately.

In `FrameReader`, I set `frame->Standards` to `NULL`, configure `frame->skb`, and then set `frame->IEE802Buffer`. In Netfilter, if we return `0`, it indicates success; if we return `1`, we instruct Netfilter to drop the packet. To handle this, I created `static int CloseFrame(struct Frame*frame);`, which does not drop the packet but closes it. Additionally, `static int DropFrame(struct Frame*frame);` drops the packet without closing it. To handle both dropping and closing in one step, I implemented `static int DropAndCloseFrame(struct Frame*frame);`.

Now, you see `Standards` in the `Frame`. What does it mean? It is a simple and efficient way to point to an address within `frame->IEE802Buffer`. This design simplifies coding by avoiding heavy logic, enabling faster responses to the caller. The goal is to minimize processing time while maintaining flexibility in handling the data.

We can pass the `FrameReader` function's `struct Frame*frame` to `static int IEEE802Reader(struct Frame*frame)`. In this section, we simply read the data from the frame and perform the necessary action. 

This design keeps the code modular and focused, allowing `FrameReader` to handle incoming frames while delegating the specific task of processing IEEE 802.3 frames to `IEEE802R`. This approach simplifies the overall logic and enhances readability while maintaining efficiency.

We don't know if the server is busy, but we respect the server's memory. For this reason, I’ve established some ground rules for the kernel. `CreateStandard` returns `false` if the server is busy, couldn't create the standard, or if the standard already exists. This is why it’s important to call it only when needed. Repeated calls would just result in `false`, which makes no sense.

In `IEEE802R`, a `char* Pointer` is used to handle the result of `CreateStandard`. The ternary operator is utilized for efficiency: if `CreateStandard` returns `true`, the frame is passed to `IEEE802A` to process the IEEE 802 data. If it returns `false`, `CloseFrame(frame)` is called to clean up the frame and its associated standards.

Here’s the code:

```c
static inline int IEEE802R(struct Frame* frame) {
    char* Pointer;
    return CreateStandard(frame,802,0,&Pointer,0)?IEEE802A(frame,(struct IEEE802_3*)Pointer):CloseFrame(frame);
}
```

We will discuss `IEEE802A` later; first, let’s focus on the structure:

```c
struct IEEE802 {
    unsigned char DMAC[6],SMAC[6],ET[2];
};
```

Here:
- **DMAC** refers to the Destination Media Access Control address.
- **SMAC** refers to the Source Media Access Control address.
- **ET** refers to EtherType.

When processing the frame, the DMAC indicates the destination, and the SMAC represents the source from where the frame originated.

#  MSB (Most Significant Bit) and LSB (Least Significant Bit).

In binary systems, every number is represented as a series of bits. Each bit has a specific position and weight, contributing to the overall value of the number. Two key terms often encountered are the **Most Significant Bit (MSB)** and the **Least Significant Bit (LSB)**.


#### **What is MSB?**
The **Most Significant Bit (MSB)** is the **leftmost bit** in a binary number. It holds the highest positional value and often indicates the largest contribution to the number’s total value.

For instance:
- In an 8-bit binary number, the MSB represents \(2^7\), which is 128 in decimal.
- In signed numbers, the MSB commonly determines the **sign** (0 for positive, 1 for negative).



#### **What is LSB?**
The **Least Significant Bit (LSB)** is the **rightmost bit** in a binary number. It represents the smallest positional value, contributing the least to the number. It is also often used to represent **parity** or to indicate small incremental changes.

For example:
- In an 8-bit binary number, the LSB represents \(2^0\), which equals 1 in decimal.



#### **Why Are MSB and LSB Important?**
1. **Value Interpretation**:
   - The MSB determines the largest contribution to the total value.
   - The LSB influences the smallest changes in the number.

2. **Data Transmission**:
   - Some systems transmit data starting with the MSB (MSB-first) or the LSB (LSB-first).
   
3. **Error Checking**:
   - Knowing the MSB and LSB positions is crucial in identifying corrupted or altered data.


### **Table: Binary Representation and Bit Significance**

| **Binary Number** | **MSB (Most Significant Bit)** | **LSB (Least Significant Bit)** | **Decimal Value** |
|--------------------|--------------------------------|----------------------------------|-------------------|
| `10000000`         | `1`                            | `0`                              | 128               |
| `01000000`         | `0`                            | `0`                              | 64                |
| `00000001`         | `0`                            | `1`                              | 1                 |
| `11111111`         | `1`                            | `1`                              | 255               |
| `10101010`         | `1`                            | `0`                              | 170               |


#### **Real-World Applications**
1. **Signed Numbers**:
   - In signed binary numbers, the MSB is used to indicate the sign (0 for positive, 1 for negative).
   - Example: In an 8-bit signed system, `10000001` represents `-127`.

2. **Data Transmission**:
   - Protocols may specify sending the MSB first (e.g., Big Endian) or the LSB first (e.g., Little Endian).

3. **Bitmasking**:
   - The LSB is often toggled in low-level operations to represent state changes, flags, or parity.

# IEEE802->ET[2]

Here’s a corrected and refined version of your text:

---

**Understanding ET (EtherType) as a 16-bit Number**

ET (EtherType) is a **16-bit number** made up of two 8-bit values (or **bytes**): the **higher byte** and the **lower byte**. Each byte represents a value between **0 and 255**.

To calculate the full 16-bit value, follow these steps:

1. Take the **higher byte** (big box) and multiply it by 256. This gives it the proper weight in the overall 16-bit number.  
2. Add the **lower byte** (small box) directly to the result.

Here’s an example:

- Higher byte: `00001000` (8 in decimal)  
- Lower byte: `00000000` (0 in decimal)

Step 1: Multiply the higher byte by 256:  
\( 8 \times 256 = 2048 \)

Step 2: Add the lower byte:  
\( 2048 + 0 = 2048 \)

The full 16-bit value is **2048**.

Another example:

- Higher byte: `00000001` (1 in decimal)  
- Lower byte: `11111111` (255 in decimal)

Step 1: Multiply the higher byte by 256:  
\( 1 \times 256 = 256 \)

Step 2: Add the lower byte:  
\( 256 + 255 = 511 \)

The full 16-bit value is **511**.



**What Does the ET Number Represent?**

EtherType numbers are standardized, and each value represents a specific protocol. You can find a comprehensive list in the [IANA IEEE 802 Numbers Registry](https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml). 

For example:
- `2048` represents **Internet Protocol version 4 (IPv4)**.  
- `34525` represents **Internet Protocol version 6 (IPv6)**.

These references are critical for identifying the protocol associated with each EtherType.


**Finalizing the `IEEE802A` Function**

We will now finalize the `IEEE802A` function to support only the following EtherTypes:
- `2048`, `2054`, and `34525` use **RFC9542**.
- `34887` and `34888` use **RFC5332**.
- `35151` uses **RFC8300**.

Any other EtherType will result in closing the frame.

This ensures the function handles only the protocols relevant to our application. 

**RFC 9542** defines the rules for IPv6 **Neighbor Discovery Protocol (NDP)**. It describes how devices in an IPv6 network find each other, resolve IP addresses to MAC addresses, detect unreachable devices, and discover routers and network prefixes for configuration.

**RFC 5332** specifies how **MPLS (Multi-Protocol Label Switching)** labels are used to carry IPv4 and IPv6 packets. It describes the encapsulation of these packets with MPLS headers to enable efficient routing and forwarding in large-scale networks.

**RFC 8300** defines **Generic UDP Encapsulation (GUE)**, a protocol for encapsulating different types of payloads (e.g., IPv4, IPv6) over UDP. It supports features like extensibility and optional security with minimal overhead.

**RFC 9542 (IPv6 Neighbor Discovery Protocol - NDP)** directly uses MAC addresses as part of its functionality. MAC addresses are included in Neighbor Solicitation and Neighbor Advertisement messages to resolve IPv6 addresses to MAC addresses, ensuring communication at Layer 2.

**RFC 5332 (MPLS)** relies on MAC addresses for the transport of Ethernet frames carrying MPLS-labeled packets. However, MPLS itself does not interact with MAC addresses; it uses labels for forwarding decisions.

**RFC 8300 (Generic UDP Encapsulation - GUE)** includes MAC addresses in the Ethernet frame that carries the UDP-encapsulated packet. GUE operates at Layer 4, so it does not directly use MAC addresses, which are only required for delivery at Layer 2.

---

| **Aspect**                  | **RFC 9542 (NDP)**             | **RFC 5332 (MPLS)**              | **RFC 8300 (GUE)**               |
|-----------------------------|--------------------------------|----------------------------------|----------------------------------|
| **Role of MAC Address**      | Central and part of the payload | Required for Layer 2 delivery    | Required for Layer 2 delivery    |
| **Layer**                   | Layer 2 & Layer 3 (Data Link + Network) | Layer 2.5 (Between Data Link & Network) | Layer 4 (Transport over UDP)     |
| **Active Use of MAC Address** | Yes (for IP-to-MAC resolution) | No (used only by Ethernet frame) | No (used only by Ethernet frame) |
| **EtherType Involvement**    | Not specific to NDP            | 34887, 34888 indicate MPLS       | Not specific to GUE              |

