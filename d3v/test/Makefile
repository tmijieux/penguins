TARGET=demod3v
CFLAGS=-std=gnu99 -Wall -I ../../include
LDFLAGS=-lGL -lX11 -lGLU -lm -ljpeg
SRC=main.c  \
	../d3v.c \
	../scene.c \
	../model.c \
	../object.c \
	../light.c \
	../camera.c  \
	../texture.c \
	../../server/path.c \
	../../utils/math/math.c


OBJ=$(patsubst %.c, %.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJ)  *~

mrproper: clean
	$(RM) $(TARGET)
