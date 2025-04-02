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
	vector<ofVec2f> noteVelocities; // Per le palline nere (effetto molla)
	vector<float> colors;

	ofVec2f redBallPos;
	ofVec2f redBallTarget;
	float redBallInterpolation;			 // Ripristiniamo l'interpolazione per la pallina rossa
	float interpolationRedSpeed = 0.04f; // Velocità interpolazione pallina rossa

	vector<bool> activeNotes;

	ofTrueTypeFont font;
	float circleRadius = 10.f;
	float redBallRadius = 16.f;
	bool initialized = false;
	int globalTotalNotes;

	// Parametri fisici solo per le palline nere
	// float stiffness = 0.025f; // Rigidità della molla
	vector<float> stiffness;
	float damping = 0.94f; // Smorzamento
	vector<float> mass;
	// float mass = 5.0f;	   // Massa

	ofTrueTypeFont largeFont;
	int n_iteration = 0;

	int redballVisible = 0;
};