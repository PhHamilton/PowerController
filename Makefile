THIRD_PARTY  = ./third_party
DIR_CONFIG   = ${THIRD_PARTY}/config
DIR_OLED     = ${THIRD_PARTY}/oled
DIR_FONTS    = ${THIRD_PARTY}/fonts
DIR_GUI      = ${THIRD_PARTY}/gui
DIR_BIN      = ./bin

TARGET = main

OBJ_C = $(wildcard ${DIR_OLED}/*.c ${DIR_CONFIG}/*.c ${DIR_FONTS}/*.c ${DIR_GUI}/*.c) ${TARGET}.c
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

INCLUDES= -I$(DIR_CONFIG) -I$(DIR_OLED) -I$(DIR_FONTS) -I$(DIR_GUI)

# USELIB = USE_BCM2835_LIB
# USELIB = USE_WIRINGPI_LIB
USELIB = USE_DEV_LIB
DEBUG = -D $(USELIB)
ifeq ($(USELIB), USE_BCM2835_LIB)
    LIB = -lbcm2835 -lm
else ifeq ($(USELIB), USE_WIRINGPI_LIB)
    LIB = -lwiringPi -lm
else ifeq ($(USELIB), USE_DEV_LIB)
    LIB = -llgpio -lm
endif


CC = gcc
MSG = -g -O0 -Wall
CFLAGS += $(MSG) $(DEBUG)

${TARGET}: $(OBJ_O)
	$(MKDIR)
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)

${DIR_BIN}/main.o: main.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

${DIR_BIN}/%.o: ${DIR_OLED}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

${DIR_BIN}/%.o: ${DIR_FONTS}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

${DIR_BIN}/%.o:$(DIR_GUI)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(INCLUDES)

${DIR_BIN}/%.o: ${DIR_CONFIG}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	rm -f $(DIR_BIN)/*.o $(TARGET)
