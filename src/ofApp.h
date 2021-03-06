#pragma once

#include "ofMain.h"
#include "demoParticle.h"
#include "ofxGui.h"
#include "ofxMacScreenRecorder.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void resetParticles();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		particleMode currentMode;
		string currentModeStr; 

		vector <demoParticle> p;
		vector <ofPoint> attractPoints;
		vector <ofPoint> attractPointsWithMovement;
    
        void numParticlesChanged(int & numParticles);
        void repelRadiusChanged(int & repelRadius);
        void attractRadiusChanged(int & attractRadius);
        void holdRadiusChanged(int & holdRadius);
        void colorChanged(int & color);
        void randColorChanged(bool & randColor);
        void alternateForcesChanged(bool & alternateForces);
        void setColor();
        void changeForce();
    
        //ofxFloatSlider radiusInner;
        bool updating;
        bool bHide;
        ofxIntSlider numParticles;
        ofxIntSlider repelRadius;
        ofxIntSlider attractRadius;
        ofxIntSlider holdRadius;
        ofxIntSlider colorR;
        ofxIntSlider colorG;
        ofxIntSlider colorB;
        ofxToggle randColor;
        ofxToggle alternateForces;
        ofxPanel gui;
        float randColorLerp;
        void newColor();
        float alternateForcesLerp;
    
        ofSoundPlayer  beats;
        int nBandsToGet;
        float * fftSmoothed;
        void setBeatColor(float & color);
    
        ofxMacScreenRecorder recorder;
        
		
};
