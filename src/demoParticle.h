#pragma once
#include "ofMain.h"

enum particleMode{
	PARTICLE_MODE_ATTRACT = 0,
	PARTICLE_MODE_REPEL,
	PARTICLE_MODE_NEAREST_POINTS,
	PARTICLE_MODE_NOISE,
    PARTICLE_MODE_BEATS
};

class demoParticle{

	public:
		demoParticle();
		
		void setMode(particleMode newMode);	
		void setAttractPoints( vector <ofPoint> * attract );
        void setColor(int colorR,int colorG,int colorB);

        void setup();
		void reset();
		void update();
		void draw();		
		
		ofPoint pos;
		ofPoint vel;
		ofPoint frc;
    
		
		float drag;
		float uniqueVal;
		float scale;
    
        float colorLerp;
        ofColor curColor;
        ofColor newColor;
        int repelRadius; //inner radius of particle cloud
        int holdRadius; //particle cloud 'thickness'
        int attractRadius; //outer radius of particle cloud
		
		particleMode mode;
    
        ofPoint windowCenter;
        float soundVal;
        void setBeatColor(int colorH, int colorS, int colorB);
		
		vector <ofPoint> * attractPoints;
    
    
};
