#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{

	// Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;

	// int screenWidth = ofGetScreenWidth(); // Larghezza totale del monitor
	// int screenHeight = ofGetScreenHeight();

	settings.setSize(1024, 768);
	// settings.windowMode = OF_WINDOW; // can also be OF_FULLSCREEN

	settings.windowMode = OF_FULLSCREEN;

	auto window = ofCreateWindow(settings);

	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}
