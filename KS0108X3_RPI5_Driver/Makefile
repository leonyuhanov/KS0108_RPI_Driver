OBJS	= lcdScreenApp.o KS0108X3_RPI_Driver.o
SOURCE	= lcdScreenApp.cpp KS0108X3_RPI_Driver.cpp
HEADER	= KS0108X3_RPI_Driver.h
OUT	= lcdScreenApp
FLAGS	= -fpermissive -Wwrite-strings -Wint-to-pointer-cast
LFLAGS	= -lgpiod
CC	= g++

lcdScreenApp: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)