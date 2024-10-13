savedcmd_/root/we-make-software/Storage.mod := printf '%s\n'   Storage.o | awk '!x[$$0]++ { print("/root/we-make-software/"$$0) }' > /root/we-make-software/Storage.mod
