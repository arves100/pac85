.PHONY: pac85

SOURCES = Net.c Pac85.c Ui.c Utils.c \
	Ui_DetectMirror.c Ui_Main.c

CFLAGS := $(CFLAGS) -D_UNICODE -DUNICODE -D_WIN32
LDFLAGS := $(LDFLAGS) -lgdi32 -lwininet -lcomdlg32

ifeq ($(DEBUG), 1)
CFLAGS += -D_DEBUG -DDEBUG -g
OBJDIR = Debug
else
CFLAGS += -DNDEBUG -O2
OBJDIR = Release
endif

OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
TARGET = ../$(OBJDIR)/pac85.exe

default: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo Linking $@...
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	@echo Compiling $^...
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	@rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.gch $(TARGET)