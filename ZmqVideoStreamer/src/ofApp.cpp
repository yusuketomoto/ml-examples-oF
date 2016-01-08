#include "ofMain.h"
#include "ofxZmq.h"
#include "ofxPubSubOsc.h"

static const float width = 800;
static const float height = 600;
static const int zmq_port = 14000;
static const int osc_port = 14001;

class ofApp : public ofBaseApp {
    ofVideoGrabber webcam;
    ofxZmqPublisher pub;
    string caption;
    ofTrueTypeFont font;
    int count = 0;;
public:
    void setup() {
        ofBackground(0);
        webcam.setup(640, 480);
        
        font.load("/Library/Fonts/Copperplate.ttc", 20);
        pub.bind("tcp://*:" + ofToString(zmq_port));
        pub.setHighWaterMark(2);
        ofxSubscribeOsc(osc_port, "/caption", caption);
    }
    void update() {
        webcam.update();
        if (count++ % 30 == 0) {
            ofPixels pix;
            pix = webcam.getPixels();
            pix.resize(webcam.getWidth()*0.5, webcam.getHeight()*0.5);
            ofBuffer buf;
            ofSaveImage(pix, buf);
            pub.send(buf);
        }
    }
    void draw() {
        webcam.draw(0, 0, width, height);
        ofPushStyle();
        ofSetColor(0, 120);
        ofRectangle r = font.getStringBoundingBox(caption, 20, 24);
        float padding = 5;
        ofDrawRectangle(r.x-padding, r.y-padding, r.width+padding*2, r.height+padding*2);
        ofPopStyle();
        font.drawString(caption, 20, 24);
    }
};

int main() {
    ofGLFWWindowSettings s;
    s.width = width;
    s.height = height;
    s.setPosition(ofVec2f(0, 0));
    shared_ptr<ofAppBaseWindow> window = ofCreateWindow(s);
    shared_ptr<ofApp> app(new ofApp);
    ofRunApp(window, app);
    ofRunMainLoop();
}