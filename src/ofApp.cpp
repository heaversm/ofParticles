#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
    
    gui.setup();
    
    numParticles.addListener(this, &ofApp::numParticlesChanged);
    repelRadius.addListener(this, &ofApp::repelRadiusChanged);
    attractRadius.addListener(this, &ofApp::attractRadiusChanged);
    colorR.addListener(this, &ofApp::colorChanged);
    colorG.addListener(this, &ofApp::colorChanged);
    colorB.addListener(this, &ofApp::colorChanged);
    randColor.addListener(this, &ofApp::randColorChanged);
    alternateForces.addListener(this, &ofApp::alternateForcesChanged);

    gui.add(numParticles.setup("particles", 300, 10, 1500)); //MH initial, min, max
    gui.add(repelRadius.setup("repel radius", 150, 0, 350));
    gui.add(attractRadius.setup("attract radius", 350, 50, 600));
    gui.add(holdRadius.setup("hold radius", 250, 100, 500));
    gui.add(colorR.setup("R", 208,0,255));
    gui.add(colorG.setup("G", 255,0,255));
    gui.add(colorB.setup("B", 63,0,255));
    gui.add(randColor.setup("random color", false));
    gui.add(alternateForces.setup("alternate forces", false));
    randColorLerp = 0.0;
	
	//int num = 300;
    int num = numParticles;
	p.assign(num, demoParticle());
	currentMode = PARTICLE_MODE_ATTRACT;

	currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
    
    beats.load("sounds/beat.mp3");
    beats.setVolume(0.75f);
    beats.setLoop(true);
    nBandsToGet = 128;
    
    fftSmoothed = new float[8192];
    for (int i = 0; i < 8192; i++){
        fftSmoothed[i] = 0;
    }
    
    ofxMacScreenRecorderSetting setting;
    setting.recordingArea.set(0, 0, 1024, 768);
    setting.frameRate = 60.0f;
    recorder.setup(setting);

    
	resetParticles();

    
}

//--------------------------------------------------------------

void ofApp::randColorChanged(bool &randColor){
    
    updating = true;
    
    if (randColor){
        colorR = ofRandom( 0, 255 );
        colorG = ofRandom( 0, 255 );
        colorB = ofRandom( 0, 255 );
        setColor();
        randColorLerp = 0.001f;
    } else {
        randColorLerp = 0.000f;
    }
    
    updating = false;
}

void ofApp::setColor(){
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].setColor(colorR,colorG,colorB);
    }
}

void ofApp::setBeatColor(float &color){
    int colorH = ofMap(color, 0, 2, 0, 255);
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].setBeatColor(colorH,255,255);
    }
}

void ofApp::colorChanged(int &color){
    
    updating = true;
    setColor();
    updating = false;
}

//--------------------------------------------------------------

void ofApp::alternateForcesChanged(bool &alternateForces){
    
    updating = true;
    
    if (alternateForces){
        
        if (!beats.isPlaying()){
            if (currentMode == PARTICLE_MODE_ATTRACT){
                currentMode = PARTICLE_MODE_REPEL;
            } else {
                currentMode = PARTICLE_MODE_ATTRACT;
            };
            alternateForcesLerp = 0.001f;
        } else {
            currentMode = PARTICLE_MODE_REPEL;
        }
    }
    
    updating = false;
}

//--------------------------------------------------------------

void ofApp::numParticlesChanged(int &numParticles){
    
    updating = true;
    
    int curParticles = p.size();
    int particleDiff = curParticles - numParticles;
    
    if (particleDiff > 0){ //remove particles
        for(unsigned int i = 0; i < particleDiff; i++){
            p.pop_back();
        }
    } else if (particleDiff < 0){ //add particles
        for(unsigned int i = curParticles; i < numParticles; i++){
            p.push_back(demoParticle());
        }
        resetParticles();
    }
    
    updating = false;
}

//--------------------------------------------------------------

void ofApp::repelRadiusChanged(int &repelRadius){
    //std::cout << "value: " << repelRadius << endl;
    updating = true;
    
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].repelRadius = repelRadius;
    }
    
    updating = false;
}

void ofApp::attractRadiusChanged(int &attractRadius){
    
    updating = true;
    
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].attractRadius = attractRadius;
    }
    
    updating = false;
}

void ofApp::holdRadiusChanged(int &holdRadius){
    
    updating = true;
    
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].holdRadius = holdRadius;
    }
    
    updating = false;
}


//--------------------------------------------------------------
void ofApp::resetParticles(){

	//these are the attraction points used in the forth demo 
	attractPoints.clear();
	for(int i = 0; i < 4; i++){
		attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
	}
	
	attractPointsWithMovement = attractPoints;
	
	for(unsigned int i = 0; i < p.size(); i++){
		p[i].setMode(currentMode);		
		p[i].setAttractPoints(&attractPointsWithMovement);;
		p[i].reset();
	}	
}

//--------------------------------------------------------------
void ofApp::update(){

    //std::cout << "mouse: " << attractPt << ", " << windowDim << endl;
	for(unsigned int i = 0; i < p.size(); i++){
		p[i].setMode(currentMode);
		p[i].update();
	}
	
	//lets add a bit of movement to the attract points
	for(unsigned int i = 0; i < attractPointsWithMovement.size(); i++){
		attractPointsWithMovement[i].x = attractPoints[i].x + ofSignedNoise(i * 10, ofGetElapsedTimef() * 0.7) * 12.0;
		attractPointsWithMovement[i].y = attractPoints[i].y + ofSignedNoise(i * -10, ofGetElapsedTimef() * 0.7) * 12.0;
	}

    if (randColor && randColorLerp > 0.0f && randColorLerp <= 0.90f){
                randColorLerp = randColorLerp + .01;
    } else if (randColor && randColorLerp > 0.90f) {
        newColor();
    }
    
    if (alternateForces && alternateForcesLerp > 0.0f && alternateForcesLerp <= 0.90f){
        alternateForcesLerp = alternateForcesLerp + .01;
    } else if (alternateForces && alternateForcesLerp > 0.90f) {
        changeForce();
    }

    ofSoundUpdate();
    if (beats.isPlaying()){
        currentMode = PARTICLE_MODE_BEATS;
        float * val = ofSoundGetSpectrum(nBandsToGet);
        
        for (int i = 0;i < nBandsToGet; i++){
            
            // let the smoothed calue sink to zero:
            fftSmoothed[i] *= 0.96f;
            
            // take the max, either the smoothed or the incoming:
            if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
            if (val[i] > 0.1){
                
                //FORCE
                if (alternateForces){ //toggle modes
                    if (val[i] > .5){
                        currentMode = PARTICLE_MODE_ATTRACT;
                    } else {
                        currentMode = PARTICLE_MODE_REPEL;
                    }
                } else { //send forces dynamically
                    for(unsigned int i = 0; i < p.size(); i++){
                        p[i].soundVal = *val;
                        p[i].alternateForces = alternateForces;
                    }
                }
            }
        }
        
        //COLOR
        setBeatColor(*val);
        
    }
    
}

//--------------------------------------------------------------
void ofApp::changeForce(){
    if (currentMode == PARTICLE_MODE_ATTRACT){
        currentMode = PARTICLE_MODE_REPEL;
    } else {
        currentMode = PARTICLE_MODE_ATTRACT;
    };
    alternateForcesLerp = 0.01f;
}


//--------------------------------------------------------------
void ofApp::newColor(){
    
    colorR = ofRandom( 0, 255 );
    colorG = ofRandom( 0, 255 );
    colorB = ofRandom( 0, 255 );
    setColor();
    randColorLerp = 0.01f;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (!updating){
        //ofBackgroundGradient(ofColor(60,60,60), ofColor(10,10,10));
        ofBackground(ofColor(0,0,0));
        
        for(unsigned int i = 0; i < p.size(); i++){
            p[i].draw();
        }
        
        ofSetColor(190);
        if( currentMode == PARTICLE_MODE_NEAREST_POINTS ){
            for(unsigned int i = 0; i < attractPoints.size(); i++){
                ofNoFill();
                ofDrawCircle(attractPointsWithMovement[i], 10);
                ofFill();
                ofDrawCircle(attractPointsWithMovement[i], 4);
            }
        }
        
        ofSetColor(230);	
        //ofDrawBitmapString(currentModeStr + "\n\nSpacebar to reset. \nKeys 1-4 to change mode.", 10, 20);
        
        if(!bHide){
            gui.draw();
        }

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //MODE
	if( key == '1'){
		currentMode = PARTICLE_MODE_ATTRACT;
		currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse"; 		
	}
	if( key == '2'){
		currentMode = PARTICLE_MODE_REPEL;
		currentModeStr = "2 - PARTICLE_MODE_REPEL: repels from mouse"; 				
	}
	if( key == '3'){
		currentMode = PARTICLE_MODE_NEAREST_POINTS;
		currentModeStr = "3 - PARTICLE_MODE_NEAREST_POINTS: hold 'f' to disable force"; 						
	}
	if( key == '4'){
		currentMode = PARTICLE_MODE_NOISE;
		currentModeStr = "4 - PARTICLE_MODE_NOISE: snow particle simulation"; 						
		resetParticles();
    }
    if(key == 'a'){
        bHide = !bHide;
    }
    
    //FORCES
    if(key == 'f'){ //toggle alternate forces
        alternateForces = !alternateForces;
        bool doAlternateForces = &alternateForces;
        alternateForcesChanged(doAlternateForces);
    }
    if(key == 'h'){
        if (holdRadius < holdRadius.getMax() - 10){
            holdRadius = holdRadius+10;
        } else {
            holdRadius = holdRadius.getMin();
        }
        
    }
    if(key == 'i'){
        if (repelRadius < repelRadius.getMax() - 10){
            repelRadius = repelRadius+10;
        } else {
            repelRadius = repelRadius.getMin();
        }
        
    }
    if(key == 'o'){
        if (attractRadius < attractRadius.getMax() - 10){
            attractRadius = attractRadius+10;
        } else {
            attractRadius = attractRadius.getMin();
        }
    }
    
    
    //COLOR
    if(key == 'c'){ //toggle random color
        randColor = !randColor;
        bool doRandomColor = &randColor;
        randColorChanged(doRandomColor);
    }
    if(key == 'r'){
        if (colorR < 245){
            colorR = colorR+10;
        } else {
            colorR = 0;
        }
    }
    if(key == 'g'){
        if (colorG < 245){
            colorG = colorG+10;
        } else {
            colorG = 0;
        }
    }
    if(key == 'b'){
        if (colorB < 245){
            colorB = colorB+10;
        } else {
            colorB = 0;
        }
    }
    
    //MUSIC
    if (key == 'm'){
        if (beats.isPlaying()){
            beats.stop();
        } else {
            beats.play();
        }
    }
    
    if (key == 'z'){
        if(recorder.isRecordingNow()){
            recorder.stop();
        } else {
            recorder.start(ofToDataPath("test"));
        }
    }
    
    //RESET
	if( key == ' ' ){
		resetParticles();
	}
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
