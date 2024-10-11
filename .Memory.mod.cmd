savedcmd_/root/we-make-software/Memory.mod := printf '%s\n'   Memory.o | awk '!x[$$0]++ { print("/root/we-make-software/"$$0) }' > /root/we-make-software/Memory.mod
