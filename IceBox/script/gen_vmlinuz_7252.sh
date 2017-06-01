WORKSPACE=$1
ROOTFS=$2
CROSS_COMPILER=$3
CROSS_COMPILER_DIR=`dirname \`dirname \\\`which ${CROSS_COMPILER}gcc\\\`\``
#TODO
CROSS_TOOLCHAINS=${CROSS_COMPILER_DIR}/lib/gcc/arm-linux-gnueabihf/4.8.4

_GetChar()
{
    echo ""
    echo ""
    # read n;
}

cd ${WORKSPACE}

echo "/bin/sh scripts/gen_initramfs_list.sh -o usr/initramfs_data.cpio.gz  -u 0  -g 0  $ROOTFS"
_GetChar
/bin/sh scripts/gen_initramfs_list.sh -o usr/initramfs_data.cpio.gz  -u 0  -g 0  $ROOTFS

echo "${CROSS_COMPILER}gcc -Wp,-MD,usr/.initramfs_data.o.d  -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated  -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -DINITRAMFS_IMAGE="usr/initramfs_data.cpio.gz"   -c -o usr/initramfs_data.o usr/initramfs_data.S"
_GetChar
#-msoft-floagdwarf-2  
${CROSS_COMPILER}gcc -Wp,-MD,usr/.initramfs_data.o.d  -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated  -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -DINITRAMFS_IMAGE="usr/initramfs_data.cpio.gz"   -c -o usr/initramfs_data.o usr/initramfs_data.S

echo "${CROSS_COMPILER}ld -EL -r -o usr/built-in.o usr/initramfs_data.o"
_GetChar
${CROSS_COMPILER}ld -EL -r -o usr/built-in.o usr/initramfs_data.o

echo "${CROSS_COMPILER}ld -EL -r -o vmlinux.o arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group"
_GetChar
${CROSS_COMPILER}ld -EL -r -o vmlinux.o arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group

echo "${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o .tmp_vmlinux1 -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group"
_GetChar
${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o .tmp_vmlinux1 -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group

echo "${CROSS_COMPILER}nm -n .tmp_vmlinux1  |  scripts/kallsyms > .tmp_kallsyms1.S"
_GetChar
${CROSS_COMPILER}nm -n .tmp_vmlinux1  |  scripts/kallsyms > .tmp_kallsyms1.S

echo "${CROSS_COMPILER}gcc -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -msoft-float -Wa,--gdwarf-2 -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -c -o .tmp_kallsyms1.o -x assembler-with-cpp .tmp_kallsyms1.S"
_GetChar
${CROSS_COMPILER}gcc -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -msoft-float -Wa,--gdwarf-2 -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -c -o .tmp_kallsyms1.o -x assembler-with-cpp .tmp_kallsyms1.S


echo "${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o .tmp_vmlinux2 -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group .tmp_kallsyms1.o"
_GetChar
${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o .tmp_vmlinux2 -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group .tmp_kallsyms1.o

echo "${CROSS_COMPILER}nm -n .tmp_vmlinux2  |  scripts/kallsyms > .tmp_kallsyms2.S"
_GetChar
${CROSS_COMPILER}nm -n .tmp_vmlinux2  |  scripts/kallsyms > .tmp_kallsyms2.S

echo "${CROSS_COMPILER}gcc -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -msoft-float -Wa,--gdwarf-2 -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -c -o .tmp_kallsyms2.o -x assembler-with-cpp .tmp_kallsyms2.S"
_GetChar
${CROSS_COMPILER}gcc -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -msoft-float -Wa,--gdwarf-2 -nostdinc -isystem $CROSS_TOOLCHAINS/include -Iarch/arm/include -Iarch/arm/include/generated -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -c -o .tmp_kallsyms2.o -x assembler-with-cpp .tmp_kallsyms2.S

echo "${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o vmlinux -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group .tmp_kallsyms2.o"
_GetChar
${CROSS_COMPILER}ld -EL -p --no-undefined -X --build-id -o vmlinux -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o init/built-in.o --start-group usr/built-in.o arch/arm/vfp/built-in.o arch/arm/kernel/built-in.o arch/arm/mm/built-in.o arch/arm/common/built-in.o arch/arm/net/built-in.o arch/arm/crypto/built-in.o arch/arm/firmware/built-in.o arch/arm/mach-bcm/built-in.o kernel/built-in.o mm/built-in.o fs/built-in.o ipc/built-in.o security/built-in.o crypto/built-in.o block/built-in.o arch/arm/lib/lib.a lib/lib.a arch/arm/lib/built-in.o lib/built-in.o drivers/built-in.o sound/built-in.o firmware/built-in.o net/built-in.o --end-group .tmp_kallsyms2.o

echo "${CROSS_COMPILER}objcopy -O binary -R .comment -S  vmlinux arch/arm/boot/Image"
_GetChar
${CROSS_COMPILER}objcopy -O binary -R .comment -S  vmlinux arch/arm/boot/Image

echo "cat arch/arm/boot/compressed/../Image | gzip -n -f -9 > arch/arm/boot/compressed/piggy.gzip"
_GetChar
cat arch/arm/boot/compressed/../Image | gzip -n -f -9 > arch/arm/boot/compressed/piggy.gzip

echo "${CROSS_COMPILER}gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem ${CROSS_TOOLCHAINS}/include -Iarch/arm/include -Iarch/arm/include/generated  -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -Wa,--gdwarf-2     -DZIMAGE     -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S"
_GetChar
${CROSS_COMPILER}gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem ${CROSS_TOOLCHAINS}/include -Iarch/arm/include -Iarch/arm/include/generated  -Iinclude -Iarch/arm/include/uapi -Iarch/arm/include/generated/uapi -Iinclude/uapi -Iinclude/generated/uapi -include include/linux/kconfig.h -D__KERNEL__ -mlittle-endian  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -Wa,--gdwarf-2     -DZIMAGE     -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S

BSS_SIZE=`${CROSS_COMPILER}size vmlinux | awk 'END{printf $3}'`

echo "${CROSS_COMPILER}ld -EL --defsym _kernel_bss_size=$BSS_SIZE -p --no-undefined -X -T arch/arm/boot/compressed/vmlinux.lds arch/arm/boot/compressed/head.o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/misc.o arch/arm/boot/compressed/decompress.o arch/arm/boot/compressed/debug.o arch/arm/boot/compressed/string.o arch/arm/boot/compressed/hyp-stub.o arch/arm/boot/compressed/fdt_rw.o arch/arm/boot/compressed/fdt_ro.o arch/arm/boot/compressed/fdt_wip.o arch/arm/boot/compressed/fdt.o arch/arm/boot/compressed/atags_to_fdt.o arch/arm/boot/compressed/lib1funcs.o arch/arm/boot/compressed/ashldi3.o arch/arm/boot/compressed/bswapsdi2.o -o arch/arm/boot/compressed/vmlinux "
_GetChar
${CROSS_COMPILER}ld -EL --defsym _kernel_bss_size=$BSS_SIZE -p --no-undefined -X -T arch/arm/boot/compressed/vmlinux.lds arch/arm/boot/compressed/head.o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/misc.o arch/arm/boot/compressed/decompress.o arch/arm/boot/compressed/debug.o arch/arm/boot/compressed/string.o arch/arm/boot/compressed/hyp-stub.o arch/arm/boot/compressed/fdt_rw.o arch/arm/boot/compressed/fdt_ro.o arch/arm/boot/compressed/fdt_wip.o arch/arm/boot/compressed/fdt.o arch/arm/boot/compressed/atags_to_fdt.o arch/arm/boot/compressed/lib1funcs.o arch/arm/boot/compressed/ashldi3.o arch/arm/boot/compressed/bswapsdi2.o -o arch/arm/boot/compressed/vmlinux 

echo "${CROSS_COMPILER}objcopy -O binary -R .comment -S  arch/arm/boot/compressed/vmlinux arch/arm/boot/zImage"
_GetChar
${CROSS_COMPILER}objcopy -O binary -R .comment -S  arch/arm/boot/compressed/vmlinux arch/arm/boot/zImage

cp -f arch/arm/boot/zImage vmlinuz

#echo "$MKUP -model HY7252 -pcb 1 -force -chipset 0x7445D0 -date 'Tue Mar 24 11:31:54 CST 2015' -author 'root' -major 1 -minor 0 -svn 34 -target kernel0 vmlinuz > test.bin"
#_GetChar
#$MKUP -model HY7252 -pcb 1 -force -chipset 0x7445D0 -date 'Tue Mar 24 11:31:54 CST 2015' -author 'root' -major 1 -minor 0 -svn 34 -target kernel0 vmlinuz > test.bin

cd -
