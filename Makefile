# LhAILa - minimal host-side build skeleton for vbcc / AmigaOS 3.x

CC      = vc
CFLAGS  = -c -O2 +aos68k -DAMIGA -D__AMIGA__ -I"$(AMIGA_NDK_HEADERS_INCLUDE)" -Iinclude
OUTDIR  = build

SRC = \
	src/core/archive_open.c \
	src/core/archive_iter.c \
	src/core/header_level0.c \
	src/core/header_level1.c \
	src/core/io.c \
	src/core/profile.c \
	src/core/crc.c \
	src/core/path.c \
	src/methods/registry.c \
	src/methods/lh0.c \
	src/sinks/sink_test.c \
	src/sinks/sink_file.c \
	src/orchestration/process_entry.c \
	src/orchestration/test_archive.c \
	src/orchestration/extract_archive.c \
	src/library/api_lifecycle.c \
	src/library/api_iter.c \
	src/library/api_info.c \
	src/library/api_test.c \
	src/library/api_extract.c \
	src/tools/lhaila/main.c \
	src/tools/lhaila/cmd_list.c \
	src/tools/lhaila/cmd_test.c \
	src/tools/lhaila/cmd_extract.c \
	src/tools/lhaila/textout.c

OBJ = $(SRC:%.c=$(OUTDIR)/%.o)
LD = vc
LDFLAGS = +aos68k -o=$(TARGET)
TARGET = $(OUTDIR)/lhaila

all: dirs objects link

dirs:
	mkdir -p $(OUTDIR)/src/core
	mkdir -p $(OUTDIR)/src/methods
	mkdir -p $(OUTDIR)/src/sinks
	mkdir -p $(OUTDIR)/src/orchestration
	mkdir -p $(OUTDIR)/src/library
	mkdir -p $(OUTDIR)/src/tools/lhaila

objects: $(OBJ)

link: $(TARGET)

$(TARGET): $(OBJ)
	$(LD) +aos68k -o=$@ $(OBJ)

$(OUTDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o=$@ $<

clean:
	rm -rf $(OUTDIR)

print-src:
	@printf "%s\n" $(SRC)
