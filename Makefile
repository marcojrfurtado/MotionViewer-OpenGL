FLAGS=-Wall
LOCAL_LIBRARY=joint.cpp camera.cpp motion.cpp
all:
	g++ $(FLAGS) -o motionviewer motionviewer.cpp $(LOCAL_LIBRARY)  -lglut -lGLEW -lGL -lGLU -lX11 -lm -g
clean:
	rm motionviewer
