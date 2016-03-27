RPM_SPEC_FILES.dom0 := $(if $(filter $(DIST_DOM0), $(DISTS_VM)),, rpm_spec/gui-common.spec)
RPM_SPEC_FILES.vm := rpm_spec/gui-common.spec
RPM_SPEC_FILES := $(RPM_SPEC_FILES.$(PACKAGE_SET))

ARCH_BUILD_DIRS := archlinux
DEBIAN_BUILD_DIRS := debian
WIN_COMPILER := custom
WIN_BUILD_CMD := true
WIN_SOURCE_SUBDIRS := .
WIN_OUTPUT_HEADERS := include
WIN_SIGN_CMD := true
WIN_PACKAGE_CMD := true
