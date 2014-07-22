#include "ofApp.h"

static bool shouldRemove(ofPtr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape.get()->getPosition());
}

//--------------------------------------------------------------
void ofApp::setup(){
    if( !ofFile::doesFileExist("11to16.bin") ){
        ofSystemAlertDialog("Make sure you have 11to16.bin, xTable.bin and zTable.bin in your data folder!");
        ofExit(); 
    }

    ofBackground(30, 30, 30);
    
    
    offsetX.set("Offset X", -420,-650,50);
    offsetY.set("Offset Y", -80,-250,250);
    minBlobSize.set("Min Blob", 3000,30,4000);
    blurAmt.set("Blur Amt", 5,0,40);
    
    
    panel.setup("distance in mm", "settings.xml", 20, 20);
    panel.add(kinect.minDistance);
    panel.add(kinect.maxDistance);
    panel.add(offsetX);
    panel.add(offsetY);
    panel.add(minBlobSize);
    panel.add(blurAmt);
    panel.loadFromFile("settings.xml");

    kinect.open();
    
    nearThreshold = 1.0;
    farThreshold = 0.9;
    
    depthWidth = 512;
    depthHeight = 424;
    
    grayKinect.allocate(512, 424);
    floatKinect.allocate(512,424);
    
    surfboard.loadImage("surfboard_700px.png");
    
    sourceFbo.allocate(ofGetWidth(), ofGetHeight());
    
    ciBase.setup();
    ciBlur.setup(ofGetWidth(), ofGetHeight(), ciBase.glCIcontext);
    
    // Box2d
	box2d.init();
	box2d.setGravity(0, 20);
	box2d.createGround();
	box2d.setFPS(30.0);
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    if( kinect.isFrameNew() ){
        texDepth.loadData( kinect.getDepthPixels() );
        texRGB.loadData( kinect.getRgbPixels() );
        grayKinect.setFromPixels(kinect.getDepthPixels());

        contourFinder.findContours(grayKinect, minBlobSize, (depthWidth*depthHeight)/3, 10, 2);
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    // add some circles every so often
	if((int)ofRandom(0, 50) == 0) {
        ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
        circle.get()->setPhysics(0.3, 0.5, 0.1);
        circle.get()->setBounce(0.8);
		circle.get()->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-100, 100), -20, ofRandom(30, 60));
		circles.push_back(circle);
	}
	
    ofRemove(circles, shouldRemove);
    
	box2d.update();
}


//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(255);

    ofSetColor(0);
    //capture stuff for core image processing
    sourceFbo.begin();
    ofClear(0);
    //clear the polyline on each loop
    kinectLine.clear();

    for(int i=0; i<contourFinder.blobs.size(); i++){
        ofSetColor(0);
        ofBeginShape();

        if(i==0){ //lets only look at the first/largest blob, shall we?
            bottomY = ofGetHeight();
            topY=0;
            vector<ofPoint> mapPts;
            for (int j=0; j<contourFinder.blobs[i].nPts; j++) {
                
                ofPoint mapPt, mapCent;
                mapPt.x = ofMap(contourFinder.blobs[i].pts[j].x, 0, depthWidth, 0, ofGetWidth());
                mapPt.y = ofMap(contourFinder.blobs[i].pts[j].y, 0, depthHeight, 0, ofGetHeight());

                ofVertex(mapPt.x, mapPt.y); //add vertices to shape
                //find bottom of the shape
                if (mapPt.y<bottomY) {
                    bottomY = mapPt.y;
                }
                //find top of the shape
                if(mapPt.y>topY){
                    topY = mapPt.y;
                }
                mapPts.push_back(mapPt);

                
            }
            ofEndShape();
            
            //map centroid points
            mapCent.x = ofMap(contourFinder.blobs[i].centroid.x,0, depthWidth, 0, ofGetWidth());
            mapCent.y = ofMap(contourFinder.blobs[i].centroid.y,0, depthHeight, 0, ofGetHeight());
            kinectLine.addVertices(mapPts); //add vertices to the box2d edge object
            kinectLine.create(box2d.getWorld()); //create it
            //Need to stop errors in console for "body is not defined"
        }
    }
    

    int blobHeight;
    blobHeight = topY-bottomY; //can use this to get a hacky rough placement and scale of surfboard - can improve with skeleton
    surfboard.draw(mapCent.x+offsetX, mapCent.y+offsetY, ofMap(blobHeight,0,ofGetHeight(), 0.3, 1)*surfboard.getWidth(),ofMap(blobHeight,0,ofGetHeight(), 0.3, 1)* surfboard.getHeight());
    //draw beachballs
    for (int i=0; i<circles.size(); i++) {
		ofFill();
		ofSetColor(50);
		circles[i].get()->draw();
	}
    
    sourceFbo.end();
    
    ofSetColor(255);
    
    //apply core image effects and draw fbo
    ciBlur.setRadius(blurAmt);
    ciBlur.update(sourceFbo.getTextureReference());
    ciBlur.draw(0, 0);
    
    if (bDebug) {
        
        ofSetColor(255, 0, 0);
        ofLine(0, topY, ofGetWidth(), topY);
        ofLine(0, bottomY, ofGetWidth(), bottomY);
        ofDrawBitmapString("Bottom Y: " +ofToString(topY), mapCent.x, mapCent.y);
        ofDrawBitmapString("Top Y: " +ofToString(bottomY), mapCent.x, mapCent.y+20);
        ofDrawBitmapString("Height:"+ofToString(blobHeight), mapCent.x, mapCent.y+40);

        kinectLine.draw();
        panel.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='d'){
        bDebug = !bDebug;
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
