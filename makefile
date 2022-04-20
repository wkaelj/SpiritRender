# cmake wrapper

DEBUGGER = gdb -tui

BUILDFDR = build
EXEC = SpiritRender

default:
	make -C $(BUILDFDR)/

run:
	./$(BUILDFDR)/$(EXEC)

debug:
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

# use dmalloc sometimes because memory is hard
dmalloc:
	dmalloc -gdb -V -l malloclog -i 1 high
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)/Config/shaders
	cp Config/*.txt $(BUILDFDR)/Config/
	cmake -B $(BUILDFDR)