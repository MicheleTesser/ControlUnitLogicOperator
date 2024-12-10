CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

cores_src_path := $(or $(CORES_ROOT), .)
can.c := $(cores_src_path)/can.c

ifndef $(CORE_0_ROOT)
CORE_0_ROOT := $(cores_src_path)/core_0
include $(CORE_0_ROOT)/Makefile
endif

ifndef $(CORE_1_ROOT)
CORE_1_ROOT := $(cores_src_path)/core_1
include $(CORE_1_ROOT)/Makefile
endif

ifndef $(CORE_2_ROOT)
CORE_2_ROOT := $(cores_src_path)/core_2
include $(CORE_2_ROOT)/Makefile
endif
