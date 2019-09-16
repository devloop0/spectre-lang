BUILD_DIR=build
ASM_DIR=$(BUILD_DIR)/asm
O_DIR=$(BUILD_DIR)/o

STATIC_LIBRARY=libspectre.a

FILES=$(shell find . -name '*.sp' -type f -printf '%P\n')
OBJECTS=$(patsubst %.sp, $(O_DIR)/%.o, $(subst /,!,$(FILES)))
ASSEMBLIES=$(patsubst %.sp, $(ASM_DIR)/%.s, $(subst /,!,$(FILES)))

all: setup $(STATIC_LIBRARY)

$(STATIC_LIBRARY): $(OBJECTS)
	ar rcs $@ $^

$(OBJECTS): $(O_DIR)/%.o: $(ASM_DIR)/%.s
	as -mfloat-abi=hard -mfpu=vfp $^ -o $@

.SECONDEXPANSION:
$(ASSEMBLIES): $(ASM_DIR)/%.s: ./$$(subst !,/,%.sp)
	spectre $^
	mv $(patsubst %.sp, %.s, $^) $@

clean:
	rm -rf $(BUILD_DIR) $(STATIC_LIBRARY)

setup:
	mkdir -p $(ASM_DIR) $(O_DIR)
