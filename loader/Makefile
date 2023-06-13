

TARGET:=Loader
SRC=loader.cpp
OBJ=$(SRC:.cpp=.o)
CXX:=g++
CXXFLAGS:=-std=c++17

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)	

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

