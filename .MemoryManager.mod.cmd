savedcmd_/root/we-make-software/MemoryManager.mod := printf '%s\n'   MemoryManager.o | awk '!x[$$0]++ { print("/root/we-make-software/"$$0) }' > /root/we-make-software/MemoryManager.mod
