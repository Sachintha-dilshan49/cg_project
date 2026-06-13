# ============================================================
#  Makefile  -  Build the whole project
#  Run:  make          (builds the project)
#        make run      (builds and runs)
#        make clean    (removes compiled files)
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -O2

# Detect OS and set OpenGL flags
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # macOS
    GL_FLAGS = -framework OpenGL -framework GLUT
else
    # Linux
    GL_FLAGS = -lGL -lGLU -lglut
endif

TARGET  = cg_project
SRCS    = main.cpp \
          utils/profpixel.cpp \
          scenes/scene_intro.cpp \
          scenes/scene1.cpp \
          scenes/scene2.cpp \
          scenes/scene3.cpp \
          scenes/scene4.cpp \
          scenes/scene5.cpp \
          scenes/scene_outro.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GL_FLAGS)

%.o: %.cpp common.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
