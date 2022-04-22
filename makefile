# cmake wrapper

DEBUGGER = gdb

BUILDFDR = build
EXEC = SpiritRender

default:
	make config=debug verbose=1 -C $(BUILDFDR)/

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
	mkdir -p $(BUILDFDR)/Config/shaders
	cp Config/*.txt $(BUILDFDR)/Config/
	cmake -B $(BUILDFDR)