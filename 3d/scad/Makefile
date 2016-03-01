# STLs to build
PARTS := u side body_bottom body_top head leg

# Output directory
BUILD_DIR := build

# Targets are $(BUILD_DIR)/file.stl
TARGETS := $(addprefix $(BUILD_DIR)/, $(addsuffix .stl,$(PARTS)))

all: $(TARGETS)

build_dir:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.stl: metabot/print/%.scad build_dir
	@echo "Building $@..."
	@openscad $< -o $@ 

view:
	@openscad metabot/metabot.scad -o metabot.png --imgsize=1024,768 --camera=400,200,300,0,0,0 
	@eog metabot.png 

csg:
	@openscad metabot/metabot.scad -o metabot.csg

clean:
	rm -rf $(BUILD_DIR) *.csg movie-out
