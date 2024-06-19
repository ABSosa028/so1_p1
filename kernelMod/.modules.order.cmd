cmd_/home/parallels/Downloads/p1/kernelMod/modules.order := {   echo /home/parallels/Downloads/p1/kernelMod/p1.ko; :; } | awk '!x[$$0]++' - > /home/parallels/Downloads/p1/kernelMod/modules.order
