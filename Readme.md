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

I think `waitForMemory` is an important function, which is why I made it global throughout the system, but not in Linux. That's why I used `EXPORT_SYMBOL(waitForMemory);` for that function. However, it's different inside `IEEE802_3Out`, where we use `waitForMemoryForSKB`, which is not global and doesn't need to be. I understand if you noticed that I didn’t free the `skb`, but don’t worry—`dev_queue_xmit` will handle freeing it. No need to be concerned.


In the `Hook` function, we first check if the incoming packet is from an Ethernet device and ensure the packet length is valid. If the device isn't recognized as Ethernet or the packet doesn't meet the required conditions, it gets accepted without further processing. Otherwise, the function proceeds to pass the packet to `IEEE802_3In`, where the actual packet processing happens. A frame is created for the incoming data before handing it off to `IEEE802_3In`. After the packet is processed, it's allowed to continue through the network stack, ensuring smooth integration into the system.


In the `IEEE802_3Out` function, we first allocate memory for the packet to be sent. The function uses a loop that attempts to send the packet, retrying up to 100 times if the transmission fails. Each time a transmission attempt is made, the loop checks whether the maximum number of retries has been reached. If the transmission is successful, the packet is sent and the function returns a success status. However, if the retry limit is exceeded without success, the packet is freed from memory, and the function returns a failure status. This ensures the system makes multiple attempts to send the packet, but avoids endless retries by capping the limit at 100.

If we want learn IEEE 802.3, we can create a simple hook in Linux that will call IEEE802_3In. We also have IEEE802_3Out for outgoing packets.


WeMakeSoftware Frame basic content what come in we use the Hook to setup this.


Basic IEEE 802.3 Design

| Destination MAC address | Source MAC address | Type/Length | User Data | 
|-------------------------|--------------------|-------------|-----------|
| 6                       | 6                  | 2           | 46 - 1500 |

