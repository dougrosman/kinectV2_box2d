#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCoreImage.h"
#include "ofxBox2d.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxPanel panel;
        ofParameter<int> offsetX, offsetY, minBlobSize;
        ofxSlider <int> minDistance;
        ofxSlider <int> maxDistance;

    
        ofxKinectV2 kinect;
        ofTexture texDepth;
        ofTexture texRGB;
        ofImage surfboard;
        ofPoint mapCent;
        void threshHold(ofFloatPixels &r);
        
        float nearThreshold, farThreshold;
        
        const bool bThreshBool = false;
        
        ofxCvContourFinder contourFinder;
        ofxCvGrayscaleImage grayKinect;
        ofxCvFloatImage floatKinect;
        
        int depthWidth;
        int depthHeight;
        int topY, bottomY;
        
        //Filters
        ofxCI ciBase;
        ofxCIBlur ciBlur;
        ofParameter <float> blurAmt;
        ofFbo sourceFbo;
        
        //Physics
        ofxBox2d                            box2d;
        vector <ofPtr<ofxBox2dCircle> >     circles;
    ofxBox2dEdge kinectLine;
    
    bool bDebug;


 
    };
