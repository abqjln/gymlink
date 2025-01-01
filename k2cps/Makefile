# Made to compile on different targets so exec is unique

HOST=$(shell hostname)
TARGET = k2cps-$(HOST)

# Relies on GYMLINK_ROOT for bluez and bluez_inc
ifndef GYMLINK_ROOT
$(error GYMLINK_ROOT is not set)
endif

#########################################
# Keep objs separate
SRC_DIRS := ./ ../gl
INC_DIRS := ${SRC_DIRS}
OBJ_DIR := $(CURDIR)/$(HOST)/obj
$(shell mkdir -p $(OBJ_DIR))
DEP_DIR := $(OBJ_DIR)
$(shell mkdir -p $(DEP_DIR))

BINC_DIR := ${GYMLINK_ROOT}/bluez_inc/binc
BINC_INC_DIR := ${BINC_DIR}
BINC_OBJ_DIR := ${CURDIR}/$(HOST)/binc_obj
$(shell mkdir -p $(BINC_OBJ_DIR))
BINC_DEP_DIR := $(BINC_OBJ_DIR)
$(shell mkdir -p $(BINC_DEP_DIR))

# Search for files directory order specified above
vpath %.c ${SRC_DIRS} ${BINC_DIR}
vpath %.h ${INC_DIRS} ${BINC_INC_DIR}

# Auto-generate GLIB and GIO flags
GIOFLAGS := $(shell pkg-config --cflags --libs gio-2.0)
#$(info gio is ${GIOFLAGS})

# Compiler/linker and flags
CC = gcc
VALGRIND_FLAGS = -g -O0
BINC_WFLAGS = -Wall -Wshadow -Wstrict-prototypes
INCFLAGS ?= $(addprefix -I,$(INC_DIRS) $(BINC_INC_DIR))
# pedantic flags!
WPEDANTICFLAGS = -Wall -Wconversion -Wmissing-prototypes -Wshadow -Wstrict-prototypes -Wextra -Wno-unused-parameter -Werror -Wno-unused-function
CPPDEFINES = -DBINC_OK=0 -DCONFIG_H=\"servers_config.h\"
DEPFLAGS = -MMD -MP


#########################################
SRCS := k2cps.c \
		gl_adapter.c gl_on_adapter.c gl_agent.c gl_server.c gl_on_server.c gl_central.c gl_on_central.c gl_client.c gl_on_client.c gl_on_peripheral.c \
		gl_ble_dis.c gl_ble_bs.c gl_ble_cscs.c gl_ble_cps.c gl_ble_hrs.c gl_ble_rscs.c gl_ble_ftms.c gl_service_m3i_beacon.c gl_utilities.c
OBJS := $(addprefix ${OBJ_DIR}/,$(SRCS:.c=.o))
#
BINC_SRCS := adapter.c advertisement.c agent.c application.c characteristic.c descriptor.c device.c logger.c parser.c service.c utility.c
BINC_OBJS := $(addprefix ${BINC_OBJ_DIR}/,$(BINC_SRCS:.c=.o))

$(TARGET): $(OBJS) $(BINC_OBJS)
	$(CC) -o $@ $(OBJS) $(BINC_OBJS) $(GIOFLAGS) -lm


#########################################
# binc library special handling
$(BINC_OBJ_DIR)/adapter.o : adapter.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/advertisement.o : advertisement.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/agent.o : agent.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/application.o : application.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/characteristic.o : characteristic.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/device.o : device.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/descriptor.o : descriptor.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/logger.o : logger.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/parser.o : parser.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/service.o : service.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@
$(BINC_OBJ_DIR)/utility.o : utility.c
	$(CC) -c $< $(BINC_WFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS) -o $@


#########################################
# Default compiler with picky flags
$(OBJ_DIR)/%.o : %.c
	$(CC) -c $< -o $@ $(INCFLAGS) $(WPEDANTICFLAGS) $(CPPDEFINES) $(DEPFLAGS) $(GIOFLAGS) $(VALGRIND_FLAGS)


#########################################
# The rest
.PHONY: clean

clean:
	rm -f $(TARGET)
	rm -f $(OBJ_DIR)/*
	rm -f $(DEP_DIR)/*

cleanall:
	rm -f $(TARGET)
	rm -f $(OBJ_DIR)/*
	rm -f $(DEP_DIR)/*
	rm -f $(BINC_OBJ_DIR)/*
	rm -f $(BINC_DEP_DIR)/*

# Auto-generate dependencies with -MMD, -MP flags
-include $(OBJS:.o=.d)




