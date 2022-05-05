# cmake wrapper

DEBUGGER = gdb

BUILDFDR = bin
EXEC = SpiritRender

default:
	make --quiet -C $(BUILDFDR)

run:
	cd $(BUILDFDR)
	./$(BUILDFDR)/$(EXEC)

debug:
	cd $(BUILDFDR)
	$(DEBUGGER) -tui $(BUILDFDR)/$(EXEC)

debugngui:
	cd $(BUILDFDR)
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

# use dmalloc sometimes because memory is hard
dmalloc:
	cd $(BUILDFDR)
	dmalloc -g -V -l malloclog -i 1 high
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)
	cmake $(BUILDFDR)
