GLM_PATH=$(shell brew --prefix glm)# Change this to your glm path
GLFW_PATH=$(shell brew --prefix glfw)# Change this to your glfw path
OPENGL_LIB=-framework OpenGL# Change this to your OpenGL library

IMGUI_PATH=external/imgui
BACKENDS_PATH=$(IMGUI_PATH)/backends
SOURCE_PATH=source

CXXFLAGS = -std=c++11 -Wall
CXXFLAGS+= -I$(GLM_PATH)/include -I$(GLFW_PATH)/include -I$(IMGUI_PATH) -I$(BACKENDS_PATH) -I$(SOURCE_PATH)

LDFLAGS = $(OPENGL_LIB) -L$(GLFW_PATH)/lib -lglfw

SRC=main.cpp $(SOURCE_PATH)/_graphics.cpp $(SOURCE_PATH)/_camera.cpp \
    $(IMGUI_PATH)/imgui.cpp $(IMGUI_PATH)/imgui_draw.cpp $(IMGUI_PATH)/imgui_widgets.cpp $(IMGUI_PATH)/imgui_tables.cpp $(IMGUI_PATH)/imgui_demo.cpp \
    $(BACKENDS_PATH)/imgui_impl_glfw.cpp $(BACKENDS_PATH)/imgui_impl_opengl3.cpp

HEADERS=$(SOURCE_PATH)/_graphics.hpp $(SOURCE_PATH)/_camera.hpp \
    $(IMGUI_PATH)/imgui.h $(IMGUI_PATH)/imgui_internal.h \
    $(BACKENDS_PATH)/imgui_impl_glfw.h $(BACKENDS_PATH)/imgui_impl_opengl3.h

OBJ=$(SRC:.cpp=.o)

TARGET=final_project

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
