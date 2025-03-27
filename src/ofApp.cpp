#include "ofApp.h"

void ofApp::setup()
{
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetFrameRate(60);
    ofBackground(240);

    setupOSC();

    font.load("verdana.ttf", 12);
    largeFont.load("verdana.ttf", 72); // Carica il font grande

    redBallPos.set(-10.0f, -10.0f);
    redBallTarget = redBallPos;  // Inizializza il target con la stessa posizione
    redBallInterpolation = 1.0f; // Nessuna interpolazione iniziale
}

void ofApp::setupOSC()
{
    oscReceiver.setup(12345);
}

void ofApp::update()
{
    processOSC();

    // Interpolazione delle note
    if (initialized)
    {
        for (int i = 0; i < notePositions.size(); i++)
        {
            if (interpolationProgress[i] < 1.0f)
            {
                interpolationProgress[i] += interpolationSpeed;
                notePositions[i].y = ofLerp(notePositions[i].y, noteTargets[i].y, interpolationProgress[i]);
            }
        }
    }

    // Interpolazione pallina rossa
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
                notePositions.resize(totalNotes);
                noteTargets.resize(totalNotes);
                activeNotes.resize(totalNotes, false);
                interpolationProgress.resize(totalNotes, 1.0f);
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
                    notePositions[index].y = m.getArgAsFloat(1);
                    noteTargets[index].y = m.getArgAsFloat(2);
                    interpolationProgress[index] = 0.0f;
                    activeNotes[index] = (m.getArgAsFloat(3) > 0.5f);
                    colors[index] = m.getArgAsFloat(4) == 0.0f ? 0.0f : 160.0f;
                }
            }
        }
        // Nuovo formato per la pallina rossa: /redball x_start y_start x_end y_end
        else if (m.getAddress() == "/redball" && m.getNumArgs() >= 4)
        {
            if (m.getArgType(0) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(1) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(2) == OFXOSC_TYPE_FLOAT &&
                m.getArgType(3) == OFXOSC_TYPE_FLOAT)
            {
                // Mappa la x come per le note (se necessario)
                float x_start = ofMap(m.getArgAsFloat(0), 0, globalTotalNotes - 1, 50, ofGetWidth() - 50);
                float x_end = ofMap(m.getArgAsFloat(2), 0, globalTotalNotes - 1, 50, ofGetWidth() - 50);

                // Imposta la posizione corrente
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
    }
}

void ofApp::draw()
{
    if (!initialized)
    {
        // Centra la scritta "Infinity Bach"
        ofSetColor(0);
        string message = "Infinity Bach";
        float textWidth = largeFont.stringWidth(message);
        float textHeight = largeFont.stringHeight(message);

        largeFont.drawString(message,
                             (ofGetWidth() - textWidth) / 2,
                             (ofGetHeight() + textHeight) / 2);
        return;
    }

    // Linee di connessione
    ofSetColor(80, 80, 80, 100);
    ofSetLineWidth(1.f);
    for (size_t i = 0; i < notePositions.size() - 1; i++)
    {
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
    ofSetColor(0);
    font.drawString("Notes: " + ofToString(notePositions.size()), 20, 20);
    font.drawString("Red ball: " + ofToString(redBallPos.x, 1) + ", " + ofToString(redBallPos.y, 1), 20, 40);
    font.drawString("Target: " + ofToString(redBallTarget.x, 1) + ", " + ofToString(redBallTarget.y, 1), 20, 60);
    font.drawString("Interpolation: " + ofToString(redBallInterpolation, 2), 20, 80);
}