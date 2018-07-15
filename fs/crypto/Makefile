obj-$(CONFIG_FS_ENCRYPTION)	+= fscrypto.o

ccflags-y += -Ifs/ext4
fscrypto-y := crypto.o fname.o policy.o keyinfo.o
fscrypto-$(CONFIG_FS_CRYPTO_SEC_EXTENSION) += crypto_sec.o
