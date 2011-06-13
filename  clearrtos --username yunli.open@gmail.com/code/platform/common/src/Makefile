EXE = 
LIB = libcommon.a

INCLUDE_DIRS = \
    $(ROOT)/code/platform/common/inc \
    $(ROOT)/code/platform/kernel/arch/$(ARCH)/$(CPU)/inc \
    $(ROOT)/code/platform/kernel/core/inc \
    $(ROOT)/code/platform/kernel/driver/ctrlc/inc \

# all the error definition files should be put into ERROR_FILES variable
ERROR_FILES = \
    $(ROOT)/code/platform/common/inc/errmod.h \
    $(ROOT)/code/platform/kernel/core/inc/errtask.h \
    $(ROOT)/code/platform/kernel/core/inc/errsync.h \
    $(ROOT)/code/platform/kernel/core/inc/errdev.h \
    $(ROOT)/code/platform/kernel/core/inc/errheap.h \
    $(ROOT)/code/platform/kernel/core/inc/errmpool.h \
    $(ROOT)/code/platform/kernel/core/inc/errtmr.h \
    $(ROOT)/code/platform/kernel/arch/$(ARCH)/$(CPU)/inc/errclock.h \
    $(ROOT)/code/platform/kernel/arch/$(ARCH)/$(CPU)/inc/errcon.h \
    $(ROOT)/code/platform/kernel/driver/ctrlc/inc/errctrlc.h \
	
LINK_LIBS = 

include $(BUILD)/c.rule

$(DIR_OBJS)/error.dep: genmark

# for cleaning the errstr.def and genmark files
RMS += errstr.def genmark

genmark: $(DIR_TARGET)/err2str.exe $(ERROR_FILES)
	$(DIR_TARGET)/err2str.exe $(ERROR_FILES) > errstr.def
	@touch genmark
