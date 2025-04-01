#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetFrameRate(60);
    ofBackground(240);

    setupOSC();

    font.load("Verdana_Bold_Italic.ttf", 72);
    largeFont.load("Verdana_Bold.ttf", 108);

    redBallPos.set(-10.0f, -10.0f);
    redBallTarget = redBallPos;
    redBallInterpolation = 1.0f;
}

void ofApp::setupOSC()
{
    oscReceiver.setup(12345);
}

void ofApp::update()
{
    processOSC();

    // Fisica a molla per le palline nere
    if (initialized)
    {
        for (int i = 0; i < notePositions.size(); i++)
        {
            ofVec2f displacement = noteTargets[i] - notePositions[i];
            ofVec2f springForce = displacement * stiffness[i];
            noteVelocities[i] += springForce / mass[i];
            noteVelocities[i] *= damping;
            notePositions[i] += noteVelocities[i];
        }
    }

    // Interpolazione lineare per la pallina rossa
    if (redBallInterpolation < 1.0f)
    {
        redBallInterpolation += interpolationRedSpeed;
        redBallPos.x = ofLerp(redBallPos.x, redBallTarget.x, redBallInterpolation);
        redBallPos.y = ofLerp(redBallPos.y, redBallTarget.y, redBallInterpolation);
    }
}

void ofApp::processOSC()
{
    while (oscReceiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);

        if (m.getAddress() == "/setup" && m.getNumArgs() >= 1 && m.getArgType(0) == OFXOSC_TYPE_FLOAT)
        {
            int totalNotes = (int)m.getArgAsFloat(0);
            globalTotalNotes = totalNotes;
            if (totalNotes > 0)
            {
                stiffness.resize(totalNotes);

                for (auto &val : stiffness)
                    val = ofRandom(0.02, 0.03);

                mass.resize(totalNotes);

                for (auto &val : mass)
                    val = ofRandom(2.5f, 6.0f);

                notePositions.resize(totalNotes);
                noteTargets.resize(totalNotes);
                noteVelocities.resize(totalNotes, ofVec2f(0, 0)); // Inizializza velocità
                activeNotes.resize(totalNotes, false);
                colors.resize(totalNotes, 0.0f);

                for (int i = 0; i < totalNotes; i++)
                {
                    float x = ofMap(i, 0, totalNotes - 1, 50, ofGetWidth() - 50);
                    notePositions[i].set(x, ofGetHeight() / 2);
                    noteTargets[i].set(x, ofGetHeight() / 2);
                }
                initialized = true;
            }
        }
        else if (initialized && m.getAddress() == "/note" && m.getNumArgs() >= 4)
        {
            if (m.getArgType(0) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(1) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(2) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(3) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(4) == OFXOSC_TYPE_FLOAT)
            {
                int index = (int)m.getArgAsFloat(0);
                if (index >= 0 && index < notePositions.size())
                {
                    // Imposta solo il target, la fisica si occuperà del movimento
                    noteTargets[index].y = m.getArgAsFloat(2);
                    activeNotes[index] = (m.getArgAsFloat(3) > 0.5f);
                    colors[index] = m.getArgAsFloat(4) == 0.0f ? 0.0f : 160.0f;
                }
            }
        }
        else if (m.getAddress() == "/redball" && m.getNumArgs() >= 4)
        {
            if (m.getArgType(0) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(1) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(2) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(3) == OFXOSC_TYPE_FLOAT)
            {
                float x_start = ofMap(m.getArgAsFloat(0), 0, globalTotalNotes - 1, 50, ofGetWidth() - 50);
                float x_end = ofMap(m.getArgAsFloat(2), 0, globalTotalNotes - 1, 50, ofGetWidth() - 50);

                // Imposta la posizione corrente (inizio interpolazione)
                redBallPos.set(
                    ofClamp(x_start, 0, ofGetWidth()),
                    ofClamp(m.getArgAsFloat(1), 0, ofGetHeight()));

                // Imposta il target
                redBallTarget.set(
                    ofClamp(x_end, 0, ofGetWidth()),
                    ofClamp(m.getArgAsFloat(3), 0, ofGetHeight()));

                // Resetta l'interpolazione
                redBallInterpolation = 0.0f;
            }
        }
        else if (m.getAddress() == "/iteration" && m.getNumArgs() >= 1)
        {
            if (m.getArgType(0) == OFXOSC_TYPE_FLOAT)
            {
                n_iteration = (int)(m.getArgAsFloat(0));
            }
        }
    }
}

void ofApp::draw()
{
    if (!initialized)
    {
        // Centra la scritta "Infinity Bach"
        ofSetColor(0);
        string message = "infinity.Bach";
        float textWidth = largeFont.stringWidth(message);
        float textHeight = largeFont.stringHeight(message);

        largeFont.drawString(message,
                             (ofGetWidth() - textWidth) / 2,
                             (ofGetHeight() + textHeight) / 2);
        return;
    }

    // Linee di connessione
    // ofSetColor(80, 80, 80, 100);
    // ofSetLineWidth(1.f);

    for (size_t i = 0; i < notePositions.size() - 1; i++)
    {

        float x1 = notePositions[i].x;
        float y1 = notePositions[i].y;
        float x2 = notePositions[i + 1].x;
        float y2 = notePositions[i + 1].y;

        float dist = ofDist(x1, y1, x2, y2);
        float alpha = ofMap(dist, 10.0f, ofGetHeight() * 0.5f, 100.0f, 30.0f);
        ofSetLineWidth((alpha - 30.0f) / 70.0f * 1.4f + 0.1f);
        ofSetColor(80, alpha);

        ofDrawLine(notePositions[i], notePositions[i + 1]);
    }

    // Palline
    for (size_t i = 0; i < notePositions.size(); i++)
    {
        if (activeNotes[i])
        {
            ofSetColor(255, 0, 0, 100);
        }
        else
        {
            ofSetColor(colors[i]);
        }
        ofDrawCircle(notePositions[i], circleRadius);
    }

    // Pallina rossa (con interpolazione)
    ofSetColor(255, 0, 0, 128);
    ofDrawCircle(redBallPos, redBallRadius);

    // Debug info
    /*
    ofSetColor(0);
    font.drawString("Notes: " + ofToString(notePositions.size()), 20, 20);
    font.drawString("Red ball: " + ofToString(redBallPos.x, 1) + ", " + ofToString(redBallPos.y, 1), 20, 40);
    font.drawString("Target: " + ofToString(redBallTarget.x, 1) + ", " + ofToString(redBallTarget.y, 1), 20, 60);
    font.drawString("Interpolation: " + ofToString(redBallInterpolation, 2), 20, 80);
    */

    ofSetColor(255, 255, 255, 220);
    float s_size = font.stringWidth("iteration: ");
    font.drawString("iteration: " + ofToString(n_iteration), ofGetWidth() * 0.5f - (s_size * 0.5f), ofGetHeight() - 200.0f);
}