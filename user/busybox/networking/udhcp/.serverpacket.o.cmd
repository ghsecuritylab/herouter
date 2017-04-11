cmd_networking/udhcp/serverpacket.o := /opt/buildroot-gcc342/bin/mipsel-linux-gcc -Wp,-MD,networking/udhcp/.serverpacket.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.12.1)" -DBB_BT=AUTOCONF_TIMESTAMP -O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -I/home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include -I/home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement  -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os -Dlinux    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(serverpacket)"  -D"KBUILD_MODNAME=KBUILD_STR(serverpacket)" -c -o networking/udhcp/serverpacket.o networking/udhcp/serverpacket.c

deps_networking/udhcp/serverpacket.o := \
  networking/udhcp/serverpacket.c \
    $(wildcard include/config/feature/udhcpd/write/leases/early.h) \
  networking/udhcp/common.h \
    $(wildcard include/config/dhcpc/default/script.h) \
    $(wildcard include/config/udhcpc/slack/for/buggy/servers.h) \
    $(wildcard include/config/feature/udhcp/debug.h) \
  include/libbb.h \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/use/bb/pwd/grp.h) \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/use/bb/shadow.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/buffer.h) \
    $(wildcard include/config/ubuffer.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/inux.h) \
    $(wildcard include/config/feature/ipv6.h) \
    $(wildcard include/config/feature/check/names.h) \
    $(wildcard include/config/feature/prefer/applets.h) \
    $(wildcard include/config/busybox/exec/path.h) \
    $(wildcard include/config/getopt/long.h) \
    $(wildcard include/config/feature/pidfile.h) \
    $(wildcard include/config/feature/syslog.h) \
    $(wildcard include/config/feature/individual.h) \
    $(wildcard include/config/o.h) \
    $(wildcard include/config/ntf.h) \
    $(wildcard include/config/t.h) \
    $(wildcard include/config/l.h) \
    $(wildcard include/config/wn.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/ktop.h) \
    $(wildcard include/config/route.h) \
    $(wildcard include/config/feature/hwib.h) \
    $(wildcard include/config/debug/crond/option.h) \
    $(wildcard include/config/use/bb/crypt.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
    $(wildcard include/config/feature/editing.h) \
    $(wildcard include/config/feature/editing/history.h) \
    $(wildcard include/config/ture/editing/savehistory.h) \
    $(wildcard include/config/feature/editing/savehistory.h) \
    $(wildcard include/config/feature/tab/completion.h) \
    $(wildcard include/config/feature/username/completion.h) \
    $(wildcard include/config/feature/editing/vi.h) \
    $(wildcard include/config/feature/topmem.h) \
    $(wildcard include/config/feature/top/smp/process.h) \
    $(wildcard include/config/pgrep.h) \
    $(wildcard include/config/pkill.h) \
    $(wildcard include/config/pidof.h) \
    $(wildcard include/config/feature/devfs.h) \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config/nommu.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/byteswap.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/byteswap.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/endian.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/features.h \
    $(wildcard include/config/c99.h) \
    $(wildcard include/config/ix.h) \
    $(wildcard include/config/ix2.h) \
    $(wildcard include/config/ix199309.h) \
    $(wildcard include/config/ix199506.h) \
    $(wildcard include/config/en.h) \
    $(wildcard include/config/en/extended.h) \
    $(wildcard include/config/x98.h) \
    $(wildcard include/config/en2k.h) \
    $(wildcard include/config/gefile.h) \
    $(wildcard include/config/gefile64.h) \
    $(wildcard include/config/e/offset64.h) \
    $(wildcard include/config/d.h) \
    $(wildcard include/config/c.h) \
    $(wildcard include/config/ntrant.h) \
    $(wildcard include/config/i.h) \
    $(wildcard include/config/ern/inlines.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/uClibc_config.h \
    $(wildcard include/config/mips/isa/1//.h) \
    $(wildcard include/config/mips/isa/2//.h) \
    $(wildcard include/config/mips/isa/3//.h) \
    $(wildcard include/config/mips/isa/4//.h) \
    $(wildcard include/config/mips/isa/mips32//.h) \
    $(wildcard include/config/mips/isa/mips64//.h) \
    $(wildcard include/config/mips16ins//.h) \
    $(wildcard include/config//.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/cdefs.h \
    $(wildcard include/config/espaces.h) \
    $(wildcard include/config/tify/level.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/endian.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/arpa/inet.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/netinet/in.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/stdint.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/wchar.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/wordsize.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/types.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stddef.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/kernel_types.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/pthreadtypes.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sched.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/socket.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/limits.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/limits.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/posix1_lim.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/local_lim.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/limits.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/posix2_lim.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/xopen_lim.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/stdio_lim.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/types.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/time.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/select.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/select.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sigset.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/time.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/sysmacros.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sockaddr.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/socket.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/sockios.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/ioctl.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm-generic/ioctl.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/in.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stdbool.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/mount.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/ioctl.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/ioctls.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/ioctls.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/ioctl-types.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/ttydefaults.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/ctype.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/uClibc_touplow.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/dirent.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/dirent.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/errno.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/errno.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/errno_values.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/fcntl.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/fcntl.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sgidefs.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/uio.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/stat.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/stat.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/inttypes.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/netdb.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/rpc/netdb.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/siginfo.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/netdb.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/setjmp.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/setjmp.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/signal.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/signum.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sigaction.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sigcontext.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/sigcontext.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/types.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/64bit/phys/addr.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm-generic/int-ll64.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/bitsperlong.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm-generic/bitsperlong.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/posix_types.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/stddef.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/posix_types.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/sgidefs.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sigstack.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/ucontext.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/ucontext.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/sigthread.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/stdio.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/uClibc_stdio.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/wchar.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stdarg.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/stdlib.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/waitflags.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/waitstatus.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/alloca.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/string.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/poll.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/poll.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/mman.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/mman.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/socket.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/uio.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/time.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/wait.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/resource.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/resource.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/termios.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/termios.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/uClibc_clk_tck.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/unistd.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/posix_opt.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/environments.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/confname.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/getopt.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/utime.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/param.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/linux/param.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm/param.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/mntent.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/paths.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/sys/statfs.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/bits/statfs.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/pwd.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/grp.h \
  include/xatonum.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/netinet/udp.h \
  /home/fangan/mtk_herouter/trunk/Source/RT288x_SDK/source/lib/include/netinet/ip.h \
  networking/udhcp/dhcpc.h \
    $(wildcard include/config/ture/udhcp/port.h) \
    $(wildcard include/config/feature/udhcp/port.h) \
    $(wildcard include/config/feature/udhcpc/arping.h) \
  networking/udhcp/dhcpd.h \
    $(wildcard include/config/dhcpd/leases/file.h) \
  networking/udhcp/options.h \
    $(wildcard include/config/feature/rfc3397.h) \

networking/udhcp/serverpacket.o: $(deps_networking/udhcp/serverpacket.o)

$(deps_networking/udhcp/serverpacket.o):
