savedcmd_/root/we-make-software/wms.mod := printf '%s\n'   wms.o | awk '!x[$$0]++ { print("/root/we-make-software/"$$0) }' > /root/we-make-software/wms.mod
