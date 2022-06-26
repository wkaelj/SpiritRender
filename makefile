# cmake wrapper

DEBUGGER = gdb

BUILDFDR = bin
EXEC = SpiritRender

default:
	make -s --quiet -C $(BUILDFDR)

run:
	./$(BUILDFDR)/$(EXEC)

noshade:
	rm bin/glsl-loader-cache/*.spv

debug:
	cd $(BUILDFDR)
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

# use dmalloc sometimes because memory is hard
dmalloc:
	dmalloc -g -V -l malloclog -i 1 high
	./$(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)
	cmake $(BUILDFDR)
