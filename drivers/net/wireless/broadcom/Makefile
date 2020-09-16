obj-$(CONFIG_B43)		+= b43/
obj-$(CONFIG_B43LEGACY)		+= b43legacy/

obj-$(CONFIG_BRCMFMAC)	+= brcm80211/
obj-$(CONFIG_BRCMSMAC)	+= brcm80211/

ifneq ($(CONFIG_BCM4361),)
obj-$(CONFIG_BCM4361)  += bcmdhd_100_15/
obj-$(CONFIG_ARCH_QCOM) += bcmdhd_100_15/dhd_custom_memprealloc.o
obj-$(CONFIG_ARCH_QCOM) += bcmdhd_100_15/dhd_custom_msm.o
endif

ifneq ($(CONFIG_BCM4359),)
obj-$(CONFIG_BCM4359)  += bcmdhd_100_15/
obj-$(CONFIG_ARCH_QCOM) += bcmdhd_100_15/dhd_custom_memprealloc.o
obj-$(CONFIG_ARCH_QCOM) += bcmdhd_100_15/dhd_custom_msm.o
endif
