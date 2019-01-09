
PROJECT ?= sql2csv
MAJOR_REV := 0
MINOR_REV := 1
VERSION := ${MAJOR_REV}.${MINOR_REV}

MACH ?= native
ifeq (${MACH},native)
  TARGET  := native
  EXT     :=
  CC      := gcc
  CFLAGS  := -g 
  LDFLAGS := -g
else
ifeq (${MACH},win32)
  TARGET  := i686-pc-mingw32
  EXT     := .exe
  CC      := ${TARGET}-gcc
  CFLAGS  := -O2 
  LDFLAGS := -O2
endif
endif

CFLAGS += -Wall -std=gnu99 -pipe
BUILD_DIR   ?= ${CURDIR}/build/${MACH}
PROGRAM := ${CURDIR}/build/${PROJECT}${EXT}

PATH  := ${PATH}:/loong/native/bin:/loong/${TARGET}/bin

OBJS_APP := main.o 
OBJS := $(addprefix ${BUILD_DIR}/,${OBJS_APP})

DEPS  := $(OBJS:.o=.d)

all: ${BUILD_DIR} ${PROGRAM}

install: all

release: all
	
${PROGRAM}: ${OBJS}
	$(CC) -o $@ ${LDFLAGS} ${OBJS} ${LIBS}
	
${BUILD_DIR}/%.o: %.c
	$(CC) ${CFLAGS} -c $< -o $@

${BUILD_DIR}/%.d: %.c
	mkdir -p $(dir $@)
	$(CC) -M $(CFLAGS) $< | \
	sed 's#\($*\)\.rel[ :]*#${BUILD_DIR}/\1.rel $@ : #g' > $@

${BUILD_DIR}:
	mkdir -p $@
	
clean:
	rm -f ${BUILD_DIR}/*
	rm -f ${PROGRAM}

sinclude $(DEPS)

