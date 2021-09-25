# FIVE TA
five_fwlist := \
    five/five.mdt \
    five/five.b00 \
    five/five.b01 \
    five/five.b02 \
    five/five.b03 \
    five/five.b04 \
    five/five.b05 \
    five/five.b06 \
    five/five.b07

five_dir := firmware/five
five_signed_dir := firmware/five/signed
five_objs := $(addprefix $(obj)/,$(five_fwlist))
__fwdir := firmware
ifneq ($(fwdir),)
__fwdir := $(fwdir)
endif

$(five_objs): $(five_dir)/five.signed

five_fwabs := $(addprefix $(objtree)/,$(filter-out /%,$(__fwdir)))$(filter /%,$(__fwdir))

$(patsubst %,$(obj)/%.gen.S, $(five_fwlist)): %: $(wordsize_deps)
	$(call cmd,fwbin,$(five_fwabs)/$(patsubst $(obj)/%.gen.S,%,$@))

# The .o files depend on the binaries directly; the .S files don't.
$(patsubst %,$(obj)/%.gen.o, $(five_fwlist)): $(obj)/%.gen.o: $(__fwdir)/%

obj-y += $(patsubst %,%.gen.o, $(five_fwlist))

ifeq ($(CONFIG_FIVE_SIGN_TA),y)
# Setup five_sign_runtype and five_sign_model
include $(srctree)/firmware/five/sign_mode.mk

# The strings are came from Kconfig are quoted with \". This symbol must be removed
# because 'make' doesn't do that automatically
five_pilsplitter := $(subst $\",,$(CONFIG_FIVE_PILSPLITTER_PATH))
five_signclient := $(subst $\",,$(CONFIG_FIVE_SIGNCLIENT_PATH))


$(five_dir)/five.signed: $(five_pilsplitter) $(five_signed_dir)/five.mbn
	python $(five_pilsplitter) $(five_signed_dir)/five.mbn $(five_dir)/five
	touch $@

$(five_signed_dir)/five.mbn: $(five_signclient) $(srctree)/$(five_dir)/five.mbn
	mkdir -p $(five_dir) $(five_signed_dir)
	java -jar $(five_signclient) -model $(five_sign_model) \
		-runtype $(five_sign_runtype) -input $(srctree)/$(five_dir)/five.mbn \
		-output $(five_signed_dir)/five.mbn

else # CONFIG_FIVE_SIGN_TA

five_presigned := $(addprefix $(srctree)/$(five_dir)/presigned/,$(five_fwlist))

$(five_dir)/five.signed: $(five_presigned)
	cp $(five_presigned) $(five_dir)
	touch $@

endif # CONFIG_FIVE_SIGN_TA

clean-files += $(five_objs) $(five_dir)/five.signed
clean-dirs += $(five_signed_dir)
