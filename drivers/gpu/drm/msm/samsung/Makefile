ccflags-y := -Iinclude/drm -Idrivers/gpu/drm/msm -Idrivers/gpu/drm/msm/dsi-staging
ccflags-y += -Idrivers/gpu/drm/msm/sde
ccflags-y += -Idrivers/gpu/drm/msm/samsung

obj-y += ss_dsi_panel_sysfs.o
obj-y += ss_dsi_panel_debug.o
obj-y += ss_dsi_panel_common.o
obj-y += ss_dsi_mdnie_lite_common.o
obj-y += ss_dsi_smart_dimming_common.o
obj-y += ss_interpolation_common.o
obj-y += ss_flash_table_data_common.o

# obj-$(CONFIG_SAMSUNG_DEBUG_DISPLAY) += dlog.o
obj-$(CONFIG_DISPLAY_USE_INFO) += ss_dpui_common.o
obj-$(CONFIG_SUPPORT_POC_FLASH) += ss_ddi_poc_common.o

# TO SUPPORT SPI INTERFACE
obj-y  += ss_ddi_spi_common.o

# COPR
obj-y += ss_copr_common.o

#
# panel directory make file
#

# TO SUPPORT PBA BOOINT
obj-y += PBA_BOOTING/ss_dsi_panel_PBA_BOOTING_fhd.o

# DREAM DSC panel
obj-$(CONFIG_PANEL_S6E3HA6_AMB577MQ01_WQHD) += S6E3HA6_AMB577MQ01/

# DREAM2 DSC panel
obj-$(CONFIG_PANEL_S6E3HA6_AMS622MR01_WQHD) += S6E3HA6_AMS622MR01/

# STAR2 DSC panel
obj-$(CONFIG_PANEL_S6E3HA8_AMB622NP01_WQHD) += S6E3HA8_AMB622NP01/

# STAR DSC panel
obj-$(CONFIG_PANEL_S6E3HA8_AMB577PX01_WQHD) += S6E3HA8_AMB577PX01/

# CROWN DSC panel
obj-$(CONFIG_PANEL_S6E3HA8_AMB638RD01_WQHD) += S6E3HA8_AMB638RD01/

# LYKAN panel
obj-$(CONFIG_PANEL_S6E3FA5_AMS420QW01_FHD) += S6E3FA5_AMS420QW01/

# Self Display Should be Initialized at final
obj-y += ss_self_display_common.o