ifeq ($(DEBUG),true)
CC = g++ -g -Wall -std=c++11
VERSION = debug
else
CC = g++ -Wall -std=c++11
VERSION = release
endif

SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
BIN := $(addprefix $(BUILD_ROOT)/,$(BIN))
LINK_OBJ_DIR = $(BUILD_ROOT)/app/link_obj
DEP_DIR = $(BUILD_ROOT)/app/dep

$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(OBJS))
DEPS := $(addprefix $(DEP_DIR)/,$(DEPS))

LINK_OBJ = $(wildcard $(LINK_OBJ_DIR)/*.o)
LINK_OBJ += $(OBJS)

all:$(DEPS) $(OBJS) $(BIN)

ifneq ("$(wildcard $(DEPS))","")   
include $(DEPS)  
endif

$(BIN):$(LINK_OBJ)
	@echo "------------------------build $(VERSION) mode--------------------------------!!!"

	$(CC) -o $@ $^

$(LINK_OBJ_DIR)/%.o:%.cc
	$(CC) -I$(INCLUDE_PATH) -o $@ -c $(filter %.cc,$^)
$(DEP_DIR)/%.d:%.cc
	echo -n $(LINK_OBJ_DIR)/ > $@
	gcc -I$(INCLUDE_PATH) -MM $^ >> $@
