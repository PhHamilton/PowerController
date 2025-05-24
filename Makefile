THIRD_PARTY  = ./third_party
DIR_CONFIG   = ${THIRD_PARTY}/config
DIR_OLED     = ${THIRD_PARTY}/oled
DIR_FONTS    = ${THIRD_PARTY}/fonts
DIR_GUI      = ${THIRD_PARTY}/gui
DIR_GUI_H    = gui
DIR_ROT      = rotary_encoder
DIR_MENU     = menu_handler
DIR_OUTPUT   = output_handler
DIR_CURRENT  = current_handler
DIR_I2C      = i2c_handler
DIR_BIN      = ./bin

TARGET = main
OUT = out

OBJ_C = $(wildcard ${DIR_OLED}/*.c ${DIR_CONFIG}/*.c ${DIR_FONTS}/*.c ${DIR_GUI}/*.c ${DIR_GUI_H}/*.c ${DIR_ROT}/*.c ${DIR_MENU}/*.c ${DIR_OUTPUT}/*.c ${DIR_CURRENT}/*.c ${DIR_I2C}/*.c) ${TARGET}.c
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

INCLUDES= -I$(DIR_CONFIG) -I$(DIR_OLED) -I$(DIR_FONTS) -I$(DIR_GUI) -I$(DIR_GUI_H) -I$(DIR_ROT) -I$(DIR_MENU) -I$(DIR_OUTPUT) -I$(DIR_CURRENT) -I$(DIR_I2C) -I.

 USELIB = USE_BCM2835_LIB
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
CFLAGS += $(MSG) $(DEBUG) -MMD -MP

${TARGET}: $(OBJ_O)
	$(MKDIR)
	$(CC) $(CFLAGS) $(OBJ_O) -o ${OUT} $(LIB) -lwiringPi

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

${DIR_BIN}/%.o:$(DIR_GUI_H)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(INCLUDES)

${DIR_BIN}/%.o:$(DIR_ROT)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(INCLUDES)

${DIR_BIN}/%.o:$(DIR_MENU)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(INCLUDES)

${DIR_BIN}/%.o:$(DIR_OUTPUT)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(INCLUDES)

${DIR_BIN}/%.o: ${DIR_CONFIG}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

${DIR_BIN}/%.o: ${DIR_CURRENT}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

${DIR_BIN}/%.o: ${DIR_I2C}/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@



clean:
	rm -f $(DIR_BIN)/*.o $(TARGET)
