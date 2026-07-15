# Define the Emscripten Compiler explicitly to avoid environment sourcing errors
EMCC = ./emsdk/upstream/emscripten/emcc

# Define the C source files for Swiss Ephemeris
SWE_SRC = swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c swephlib.c swecl.c swehel.c
SWE_OBJ = $(SWE_SRC:.c=.o)
WASM_OBJ = $(SWE_SRC:.c=.wasm.o)

# Default target
all: astroc

# 1. Native Build (for testing)
libswe.a: $(SWE_OBJ)
	ar r libswe.a $(SWE_OBJ)

# Added header file dependencies here
astroc: astroc.cpp texts.h texts_te.h libswe.a
	g++ -std=c++17 -o astroc astroc.cpp libswe.a -lm

# 2. WebAssembly Build
%.wasm.o: %.c
	$(EMCC) -O3 -c $< -o $@

# Added header file dependencies here
astroc.wasm.o: astroc.cpp texts.h texts_te.h
	$(EMCC) -std=c++17 -O3 -c astroc.cpp -o astroc.wasm.o

# The Final Linker Step
wasm: astroc.wasm.o $(WASM_OBJ)
	$(EMCC) -std=c++17 -O3 $^ -o astroc.js \
	-s INVOKE_RUN=0 -s EXIT_RUNTIME=0 -s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_RUNTIME_METHODS='["callMain", "FS_createPath", "FS_writeFile"]'
	
clean:
	rm -f *.o *.wasm.o astroc astroc.js astroc.wasm astroc.data libswe.a

# ==============================================================================
# DEPLOYMENT AUTOMATION PIPELINE (UNIFIED HUGO INTEGRATION)
# ==============================================================================

HUGO_STATIC_DIR = ./static/engine

deploy: wasm
	@echo "Packaging WebAssembly build directly into Hugo Static folder..."
	@mkdir -p $(HUGO_STATIC_DIR)
	@cp index.html $(HUGO_STATIC_DIR)/
	@cp astroc.js $(HUGO_STATIC_DIR)/
	@cp astroc.wasm $(HUGO_STATIC_DIR)/
	@cp LICENSE.txt $(HUGO_STATIC_DIR)/ 
	@if [ -f astroc_part_aa.bin ]; then cp astroc_part_aa.bin $(HUGO_STATIC_DIR)/; fi
	@if [ -f astroc_part_ab.bin ]; then cp astroc_part_ab.bin $(HUGO_STATIC_DIR)/; fi
	@echo "Success! WebAssembly files compiled and placed in $(HUGO_STATIC_DIR)"

# ==============================================================================
# GIT AUTOMATION
# ==============================================================================

# Default commit message if none is provided via the command line
MSG ?= Intermediate deployment push

push: deploy
	@echo "Staging changes..."
	git add .
	@echo "Committing changes..."
	git commit -m "$(MSG)" || true
	@echo "Pushing to remote repository..."
	git push origin main
	@echo "Git push complete!"