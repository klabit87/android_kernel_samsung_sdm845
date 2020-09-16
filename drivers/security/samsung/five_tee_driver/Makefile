#
# Integrity TEE Driver
#
TEEC_CLIENT_PATH := multibuild/source/gp-api/client
PROTOCOL_PATH := multibuild/source/gp-api/protocol
FIVE_TEE_DRIVER_PATH := drivers/security/samsung/five_tee_driver

obj-$(CONFIG_FIVE_TEE_DRIVER) += itee_driver.o

itee_driver-objs += five_tee_driver.o

ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Isecurity/samsung/five \
                                     -I$(FIVE_TEE_DRIVER_PATH)/$(PROTOCOL_PATH) \
                                     -I$(FIVE_TEE_DRIVER_PATH)/$(TEEC_CLIENT_PATH)

ccflags-$(CONFIG_FIVE_USE_TRUSTONIC) += -DCONFIG_TRUSTONIC
ccflags-$(CONFIG_FIVE_USE_TZDEV) += -DCONFIG_TEEGRIS
ccflags-$(CONFIG_FIVE_USE_QSEE) += -DCONFIG_QSEE

# Trustonic
obj-$(CONFIG_FIVE_USE_TRUSTONIC) += $(TEEC_CLIENT_PATH)/tee_client_api.o \
                                    $(PROTOCOL_PATH)/teec_param_utils.o \
                                    $(TEEC_CLIENT_PATH)/teec_common_tbase.o \
                                    $(TEEC_CLIENT_PATH)/teec_operation_tbase.o \
                                    $(TEEC_CLIENT_PATH)/tee_client_api.o

ccflags-$(CONFIG_FIVE_USE_TRUSTONIC) += -I$(FIVE_TEE_DRIVER_PATH)/multibuild/include/gp-api
ifeq ($(CONFIG_FIVE_USE_TRUSTONIC), y)
  ifeq ($(CONFIG_SOC_EXYNOS7570), y)
    PLATFORM := exynos7570
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_SOC_EXYNOS7870), y)
    PLATFORM := exynos7870
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_SOC_EXYNOS7885), y)
    PLATFORM := exynos7885
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_SOC_EXYNOS9810), y)
    PLATFORM := exynos9810
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_SOC_EXYNOS9820), y)
    PLATFORM := exynos9820
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_SOC_EXYNOS9610), y)
    PLATFORM := exynos9610
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/gud/gud-$(PLATFORM)/MobiCoreDriver/public
  else ifeq ($(CONFIG_MACH_MT6768), y)
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/misc/mediatek/gud/410/MobiCoreDriver
    ccflags-$(CONFIG_FIVE_TEE_DRIVER) += -Idrivers/misc/mediatek/gud/410/MobiCoreDriver/public
  else
    $(error "Unknown SoC")
  endif
endif

# QSEE
obj-$(CONFIG_FIVE_USE_QSEE) += $(TEEC_CLIENT_PATH)/tee_client_api.o \
                               $(PROTOCOL_PATH)/teec_param_utils.o \
                               $(TEEC_CLIENT_PATH)/teec_common_qsee.o \
                               $(TEEC_CLIENT_PATH)/teec_operation_qsee.o

ccflags-$(CONFIG_FIVE_USE_QSEE) += -I$(FIVE_TEE_DRIVER_PATH)/multibuild/include/gp-api \
                                   -Idrivers/misc
aflags-$(CONFIG_FIVE_USE_QSEE) += -I$(srctree)

# Teegris
ifeq ($(CONFIG_SOC_EXYNOS7570), y)
  use_teegris_v2 := y
endif

ifeq ($(CONFIG_SOC_EXYNOS7870), y)
  use_teegris_v2 := y
endif

ifeq ($(CONFIG_SOC_EXYNOS7885), y)
  ifeq ($(CONFIG_TEEGRIS_VERSION), 3)
    ccflags-$(CONFIG_FIVE_USE_TZDEV) += -Idrivers/misc/tzdev/3.0/include/tzdev
  else
    use_teegris_v2 := y
  endif
endif

ifeq ($(use_teegris_v2), y)
  obj-$(CONFIG_FIVE_USE_TZDEV) += $(TEEC_CLIENT_PATH)/teec_common_teegris_v2.o \
                                  $(TEEC_CLIENT_PATH)/tee_client_api.o \
                                  $(PROTOCOL_PATH)/teec_param_utils.o \
                                  $(TEEC_CLIENT_PATH)/teec_operation_teegris_v2.o
  ccflags-$(CONFIG_FIVE_USE_TZDEV) += -Idrivers/misc/tzdev/include \
                                      -I$(FIVE_TEE_DRIVER_PATH)/multibuild/include/gp-api
else
  obj-$(CONFIG_FIVE_USE_TZDEV) += $(TEEC_CLIENT_PATH)/teec_operation_teegris_v3.o
  ccflags-$(CONFIG_FIVE_USE_TZDEV) += -Idrivers/misc/tzdev/include/tzdev
endif
