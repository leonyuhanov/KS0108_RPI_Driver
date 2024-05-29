src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

CFLAGS = -g

myprog: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS) -lbcm2835

.PHONY: clean
clean:
	rm -f $(obj) myprog
