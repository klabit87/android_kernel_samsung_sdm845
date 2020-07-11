ccflags-y += -Idrivers/media/platform/msm/camera/cam_utils
ccflags-y += -Idrivers/media/platform/msm/camera/cam_core
ccflags-y += -Idrivers/media/platform/msm/camera/cam_cpas/include
ccflags-y += -Idrivers/media/platform/msm/camera/cam_isp/isp_hw_mgr/include
ccflags-y += -Idrivers/media/platform/msm/camera/cam_isp/isp_hw_mgr/hw_utils/include
ccflags-y += -Idrivers/media/platform/msm/camera/cam_isp/isp_hw_mgr/hw_utils/irq_controller
ccflags-y += -Idrivers/media/platform/msm/camera/cam_isp/isp_hw_mgr/isp_hw/include
ccflags-y += -Idrivers/media/platform/msm/camera/cam_smmu/
ccflags-y += -Idrivers/media/platform/msm/camera/cam_req_mgr/
ifdef CONFIG_USE_CAMERA_HW_BIG_DATA
ccflags-y += -Idrivers/media/platform/msm/camera/cam_sensor_module/cam_sensor_utils
endif

obj-$(CONFIG_SPECTRA_CAMERA) += cam_ife_csid_dev.o cam_ife_csid_soc.o cam_ife_csid_core.o
obj-$(CONFIG_SPECTRA_CAMERA) += cam_ife_csid170.o cam_ife_csid_lite170.o
