If we establish clear ground rules for development, it will allow everyone to create the best software possible, especially in open-source environments.

By setting up a solid structure using extern, EXPORT_SYMBOL(), and properly organizing header files, you're essentially building a developer toolkit. This toolkit enables the creation of modular, reusable components that can be shared across kernel modules and easily reused in future projects. This is how you're laying the foundation for an efficient development workflow.

Having access to files within our own project is incredibly powerful. Skilled developers will understand the strength of this approach. However, this level of insight goes beyond what an amateur developer might grasp. It requires deep knowledge of modular design and kernel development, and I want you to consider that.

This software is meant to encourage everyone to use Linux. It's foundational in that regard.

Now, think about this: how much do you pay to have a university-educated professional in your company? Where I work, I earn 170 Danish kroner before tax, without pension benefits. It’s fair, because I can significantly improve the company.

I work in the IT section, and although I can develop things myself at my own pace, it’s not always clear what others can do. But if I build an open-source project, I would consider collaborating with the community. I could host the project on GitHub or another platform, and possibly gain sponsorship from developers, organizations, or companies that support open-source initiatives.

I invest in myself so I can provide the best software solutions for companies. What I can create for CleanStep, where I work, can be beneficial to others too. For anyone looking to sell products online, run promotions, or engage with social media, I can integrate these solutions into my project, providing clear and concise descriptions in the code. The code will begin with wms.c, as the company will be named "We-Make-Software." I plan to elevate "We-Make-Software" as an online-based company.

What I aim to develop is software that helps companies correct mistakes in their systems. I’m planning to build a full cloud-based system—covering storage, websites, reviews, social media, and everything needed to promote a company.

# Makefile Targets Documentation

## make play
This function pushes the changes to the repository, builds the kernel modules (`Memory.o` and `wms.o`), inserts both kernel modules (`Memory.ko` and `wms.ko`), and displays the kernel logs.

## make stop
This function stops the project by removing the `wms.ko` kernel module (and ignoring errors if it's not loaded) and cleaning up the build environment.

## make reload
This function removes both kernel modules (`wms.ko` and `Memory.ko`), cleans the build, rebuilds the modules, inserts them again, and displays the logs to ensure a fresh start.

## make insert
This function inserts both `Memory.ko` and `wms.ko` kernel modules into the system.

## make remove
This function removes the `wms.ko` kernel module first, followed by the `Memory.ko` module.

## make clean
This function cleans up the module build directory to ensure a fresh compilation.

## make all
This function builds both kernel modules (`Memory.o` and `wms.o`) by calling the kernel’s build system.

## make log
This function displays the kernel log (`dmesg`), which is useful for checking messages related to the module.

## make push
This function adds all changes, commits them with the message "Auto commit," and pushes them to the `main` branch of the repository.

## make pull
This function pulls the latest changes from the `main` branch of the repository.

## make clear
This function clears the kernel logs (`dmesg`) to ensure a clean slate before running the module.
