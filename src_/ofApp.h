#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void setupOSC();
	void processOSC();

private:
	ofxOscReceiver oscReceiver;
	vector<ofVec2f> notePositions;
	vector<ofVec2f> noteTargets;
	vector<float> interpolationProgress;
	vector<float> colors;

	ofVec2f redBallPos;
	ofVec2f redBallTarget;		// Aggiunto per il target della pallina rossa
	float redBallInterpolation; // Progresso interpolazione pallina rossa

	vector<bool> activeNotes;

	ofTrueTypeFont font;
	float circleRadius = 10.f;
	float redBallRadius = 16.f;
	bool initialized = false;
	int globalTotalNotes;
	float interpolationSpeed = 0.001f;
	float interpolationRedSpeed = 0.04f;

	ofTrueTypeFont largeFont;

	int n_iteration = 0;
};