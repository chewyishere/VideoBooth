#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOpenCV.h"
#include "ofxKinect.h"
#include "ofxAutoReloadedShader.h"
#include "ofxMidi.h"

class ofApp : public ofBaseApp, public ofxMidiListener {

    
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawShaderVideo();
    void drawMotion();
    void drawPointCloud();
    
   ofxAutoReloadedShader theShader;
   ofxAutoReloadedShader shader1;
   ofxAutoReloadedShader shader2;
   ofxAutoReloadedShader shader3;
    
    ofFbo fbo;			//Buffer for intermediate drawing
    
    void drawFbo();
    void synthesizeImage();
    ofImage image;
    
    ofVideoPlayer bgvideo;
    ofVideoPlayer 	video1;
    int filterMode;
    
    //audio
    void audioIn(float * input, int bufferSize, int nChannels);
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float smoothedVol;
    float scaledVol;
    float loudVol;
    
    ofSoundStream soundStream;
    
    
    //ofVideoGrabber video;
    ofxKinect video;
    
    ofxCvColorImage shader6_image;		//The current video frame
    
    //The current and the previous video frames as grayscale images
    ofxCvGrayscaleImage grayImage, grayImagePrev;
    
    ofxCvGrayscaleImage diff;		//Absolute difference of the frames
    ofxCvFloatImage diffFloat;		//Amplified difference images
    ofxCvFloatImage bufferFloat;	//Buffer image
    
    //ofxKinect video;
    int nearThreshold;
    int farThreshold;
    bool bThreshWithOpenCV;
    int angle;
    
    ofxSyphonServer mainOutputSyphonServer;
    

    int w = 640;
    int h = 480;
    
    ofEasyCam easyCam;
    
    
    //******** MIDI ***********
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
	void drawtheMidi();
    void drawPointCloud2();
	
	ofxMidiIn midiIn;
	ofxMidiMessage midiMessage;
    
    float midi_effect1;
    float midi_effect2;
    float midi_effect3;
    float midi_r;
    float midi_g;
    float midi_b;
    float midi_a;
    float midi_time;
    float midi_size;
    


    
};
