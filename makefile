# cmake wrapper

DEBUGGER = gdb

BUILDFDR = bin
EXEC = SpiritRender

default:
	cmake --build $(BUILDFDR)

run:
	./$(BUILDFDR)/$(EXEC)

debug:
	$(DEBUGGER) -tui $(BUILDFDR)/$(EXEC)

debugngui:
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

# use dmalloc sometimes because memory is hard
dmalloc:
	dmalloc -g -V -l malloclog -i 1 high
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)
	cmake $(BUILDFDR)
