#include "ofApp.h"
int speed;
float loudcontrol = 1.0;
float speedT;
//--------------------------------------------------------------
void ofApp::setup(){
    
    midiIn.listPorts();
	
	midiIn.openPort(0);
    
    midiIn.ignoreTypes(false, false, false);
	
	midiIn.addListener(this);
	
	midiIn.setVerbose(true);

    mainOutputSyphonServer.setName("Screen Output");
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
  
 /*
    //Show in console all details and warnings on the grabbing
    video.setVerbose(true);
    
    //Select camera by its id in system
    video.setDeviceID(0);
    
    //Select desired camera frame rate
    video.setDesiredFrameRate(60);
    
    //Start grabbing with desired frame width and height
    video.initGrabber(640,480);
    
    //Show in the console list of connected cameras
    //if you camera did not connect, please see the list
    video.listDevices();
*/
    
    //audio
    int bufferSize = 256;
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    
    bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    
	soundStream.setup(this, 2, 2, 44100, bufferSize, 4);
    ofSoundStream soundStream;
    soundStream.setDeviceID(1);
    soundStream.listDevices();
	
    //shader
    filterMode = 1;
    
    shader1.load("Shaders/filter1/ColorSplit");
    shader2.load("Shaders/filter2/liquify");
    shader3.load("Shaders/filter3/wave2");
    theShader.setMillisBetweenFileCheck(100);
    
    fbo.allocate( ofGetWidth(), ofGetHeight() );
    
    //bgvideo

    speed = 1;
    bgvideo.loadMovie("1.mov");
    bgvideo.play();
    bgvideo.setLoopState(OF_LOOP_NORMAL);
    
    

     //  ************* Kinect Stuff *************
     
     
     video.setRegistration(true);
     
     video.init();
     video.open();
     
     nearThreshold = 230;
     farThreshold = 70;
     bThreshWithOpenCV = true;
     
     ofSetFrameRate(60);
    
     video.setCameraTiltAngle(angle);
    
    
    midi_effect1 = 1;
    midi_effect2 = 1;
    midi_effect3 = 1;
    midi_r = 50;
    midi_g = 90;
    midi_b = 60;
    midi_a = 1;
    midi_size = 1;
    midi_time = 1;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    bgvideo.update();
    bgvideo.setSpeed(speed*0.5);
    
    video.update();

   if ( video.isFrameNew()) {
       synthesizeImage();
       
       //Store the previous frame, if it exists till now
        if ( grayImage.bAllocated ) {
            grayImagePrev = grayImage;
        }
        
        //Getting a new frame
        shader6_image.setFromPixels( video.getPixelsRef() );
        grayImage = shader6_image;	//Convert to grayscale image
        
        //Do processing if grayImagePrev is inited
        if ( grayImagePrev.bAllocated ) {
            //Get absolute difference
            diff.absDiff( grayImage, grayImagePrev );
            
            //We want to amplify the difference to obtain
            //better visibility of motion
            //We do it by multiplication. But to do it, we
            //need to convert diff to float image first
            diffFloat = diff;	//Convert to float image
           diffFloat *= scaledVol*100;	//Amplify the pixel values
            
            //Update the accumulation buffer
            if ( !bufferFloat.bAllocated ) {
                //If the buffer is not initialized, then
                //just set it equal to diffFloat
                bufferFloat = diffFloat;
            }
            else {
                //Slow damping the buffer to zero
                bufferFloat *= 0.85;
                //Add current difference image to the buffer
                bufferFloat += diffFloat;
            }
        }
   }
    
   	//lets scale the vol up to a 0-1 range
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    float tempVol = ofMap(scaledVol*10000,0,2000,50,250);
    ofClamp(tempVol, 50, 250);
    loudVol= tempVol*loudcontrol;
    
	volHistory.push_back( scaledVol );
	
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    drawtheMidi();
    
    drawShaderVideo();
    ofEnableAlphaBlending();
    
    switch(filterMode){
        case 1: //colorsplit
            theShader = shader1;
            break;
        
        case 2://wave1
            theShader = shader2;
            break;
            
        case 3: //wave2
            theShader = shader3;
            break;

        case 4: //timediff
            drawMotion();
            break;

        case 5://circles
            ofBackground(0,40);
            drawPointCloud();
            break;
            
        case 6://video
            ofBackground(255,255,255);
            //Draw processed image
            ofSetColor( 255, 255, 255 );
  
            image.draw( 0, 0 );
            break;
    }
    
    ofSetWindowTitle("FR" + ofToString(ofGetFrameRate())+" Vol " + ofToString(loudVol) +" midi:"+ofToString(midiMessage.control));
    ofLog() << "Framerate: " << ofGetFrameRate();
    ofLog() << "port:" << midiMessage.portNum;
    ofLog() << "control: " << midiMessage.control;
    ofLog() << "farThreshod " << farThreshold;
    ofLog() << "nearThreshod " << nearThreshold;


    mainOutputSyphonServer.publishScreen();
    

}

//--------------------------------------------------------------

void ofApp::drawShaderVideo(){
    
    float time = ofGetElapsedTimef();
    
    theShader.begin();
    theShader.setUniform1f("time", time );
    theShader.setUniform1f("vol", loudVol);
    theShader.setUniform1f("midi_effect2", midi_effect2);
    theShader.setUniform1f("midi_effect3", midi_effect3);
    theShader.setUniform1f("midi_r", midi_r);
    theShader.setUniform1f("midi_g", midi_g);
    theShader.setUniform1f("midi_b", midi_b);
    theShader.setUniform1f("midi_a", midi_a);
    theShader.setUniform1f("midi_time", midi_time);
    theShader.setUniform1f("midi_size", midi_size);
    
    video.draw(0,0,w,h);
    theShader.end();
    
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;

	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
    }
	
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}

//--------------------------------------------------------------
void ofApp::drawMotion(){
    ofBackground(0);
	if ( diffFloat.bAllocated ) {
		ofSetColor(midi_r*50,midi_g*50,midi_b*50); //
        bufferFloat.draw( 0, 0, w, h);

    }
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
    switch (key) {
            
        case '1':
            filterMode = 1;
            break;
            
        case '2':
            filterMode = 2;
            break;
            
        case '3':
            filterMode = 3;
            break;
            
        case '4':
            filterMode = 4;
            break;
            
        case '5':
            filterMode = 5;
            break;
            
        case '6':
            filterMode = 6;
            break;

    }
    
}


//--------------------------------------------------------------
void ofApp::synthesizeImage(){
    
	//Initialize output pixels
	ofPixels pixels = video.getPixelsRef();
    
	//Get pixel arrays for grabber and video
	ofPixels &pixelsGrab = video.getPixelsRef();
	ofPixels &pixelsVideo = bgvideo.getPixelsRef();
    
	//Get width and height for formulas shortening
	int w = pixelsGrab.getWidth();
	int h = pixelsGrab.getHeight();
    
	//Scan pixels
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			//Get grabber color
			ofColor colorGrab = pixelsGrab.getColor( x, y );
            
			//Shift x-coordinate by red component
			int x1 = x + ( colorGrab.r - 127);
            
			//Truncate x1 to bounds
			x1 = ofClamp( x1, 0, w-1 );
            
			//Get color of pixel (x1, y) from video
			ofColor color = pixelsVideo.getColor( x1, y );
            
            color.r = color.r*midi_r*0.5;
            color.g= color.g*midi_g*0.5;
            color.b = color.b*midi_b*0.5;
            
            
			//Set color to output pixel (x, y)
			pixels.setColor( x, y, color );
		}
	}
	//Update image pixels
	image.setFromPixels( pixels );
}

//--------------------------------------------------------------
void ofApp::drawPointCloud() {

    ofPixelsRef pixels = video.getPixelsRef();
	int step = 15;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
                ofColor color = pixels.getColor(x, y);
            
                float brightness = pixels.getColor(x, y).getBrightness();
                float rad = ofMap(brightness, 0, 255, 15, 0);
                
                float sat = pixels.getColor(x, y).getSaturation();
                float dotAlpha = ofMap(sat, 255, 0, 0,255);

                rad += ofMap(ofNoise(x, y, ofGetFrameNum()*0.01*midi_time), 0, 1, -3, 4);
            
            
                float r = color.r*midi_r;
                float g = color.g*midi_g;
                float b = color.b*midi_b;
            
                dotAlpha = dotAlpha*midi_a;
            
                ofSetColor(r,g,b, dotAlpha);

            
            float xV = x + rad * ofNoise(sin(ofGetFrameNum()*0.02));
            float yV = y + rad * ofNoise(cos(ofGetFrameNum()*0.01));
    
            ofEllipse(xV, yV, rad*midi_size*loudVol*0.003, rad*midi_size*loudVol*0.003);
            
            
			}
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
	video.setCameraTiltAngle(0); // zero the tilt on exit
	video.close();


}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    midiMessage = msg;
    
}

//--------------------------------------------------------------
void ofApp::drawtheMidi() {

    
    switch (midiMessage.control) {
            
        case 32:
            filterMode = 1;
            break;
            
        case 48:
            filterMode = 2;
            break;
            
        case 64:
            filterMode = 3;
            break;
            
        case 33:
            filterMode = 4;
            break;
            
        case 49:
            filterMode = 5;
            break;
            
        case 65:
            filterMode = 6;
            break;
            
        case 0: //size
            midi_size = ofMap(midiMessage.value, 0, 127, 1, 20);
            break;
            
        case 1: //waveX
            midi_effect2 = ofMap(midiMessage.value, 0, 127, 1, 100);
            break;
       
        case 2://waveY
            midi_effect3 = ofMap(midiMessage.value, 0, 127, 1, 100);
            break;
            
        case 3: //speed
            midi_time = ofMap(midiMessage.value, 0, 127, 0.01, 5);
            speedT = ofMap(midi_time+loudVol*0.05,1,9,0.2,3);
            break;
    
        case 6:
            nearThreshold = ofMap(midiMessage.value, 0, 127, 0, 255);
            break;
            
        case 7:
            farThreshold = ofMap(midiMessage.value, 0, 127, 0, 255);
            break;
            
        case 16:
            midi_r = ofMap(midiMessage.value, 0, 127, 0.01, 5);
            break;
            
        case 17:
            midi_g = ofMap(midiMessage.value, 0, 127, 0.01, 5);
            break;
            
        case 18:
            midi_b = ofMap(midiMessage.value, 0, 127, 0.01, 5);
            break;
            
        case 19:
            midi_a = ofMap(midiMessage.value, 0, 127, 0.1, 1);
            break;
            
        case 23:
            loudcontrol = ofMap(midiMessage.value, 0, 127, 0.5, 2);
            break;
    }
    
    
    speed = ofToInt(ofToString(speedT));
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
