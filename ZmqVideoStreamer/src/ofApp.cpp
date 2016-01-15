#include "ofMain.h"
#include "ofxPubSubOsc.h"
#include "ofxTrueTypeFontUL2.h"
#include "ofxZmq.h"

static float width = 800;
static float height = 600;
static const int zmq_port = 14000;
static const int osc_port = 14001;
static const float send_image_width = 320;

enum Mode { WEBCAM, VIDEO, IMAGE };

class ofApp : public ofBaseApp {
	ofVideoGrabber webcam;
	ofVideoPlayer video;
	ofImage image;
	ofxZmqPublisher pub;
	string caption;
	ofxTrueTypeFontUL2 font;
	int count = 0;
	Mode mode = WEBCAM;

public:
	void setup() {
		ofBackground(0);
		webcam.setup(640, 480);

		font.loadFont("/Library/Fonts/Hiragino Sans GB W6.otf", 20);
		pub.bind("tcp://*:" + ofToString(zmq_port));
		pub.setHighWaterMark(2);
		ofxSubscribeOsc(osc_port, "/caption", caption);
	}
	void update() {
		width = ofGetWidth();
		height = ofGetHeight();
		if (mode == WEBCAM)
			webcam.update();
		else if (mode == VIDEO)
			video.update();
		else
			return;

		if (count++ % 30 == 0) {
			if (mode == WEBCAM)
				sendPixels(webcam);
			else if (mode == VIDEO)
				sendPixels(video);
		}
	}
	void draw() {
		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);
		switch (mode) {
			case WEBCAM:
				drawInternal(webcam);
				break;
			case VIDEO:
				drawInternal(video);
				break;
			case IMAGE:
				drawInternal(image);
				break;
		}
		ofPopStyle();

		ofPushStyle();
		ofSetColor(0, 120);
		ofRectangle r = font.getStringBoundingBox(caption, 20, 40);
		float padding = 5;
		ofDrawRectangle(r.x - padding, r.y - padding,
                        r.width + padding * 2, r.height + padding * 2);
		ofPopStyle();
		font.drawString(caption, 20, 40);
	}
	void dragEvent(ofDragInfo info) {
		string path = info.files[0];
		string ext = ofFilePath::getFileExt(path);
		if (ext == "mp4" || ext == "mov") {
			video.load(path);
			video.play();
			mode = VIDEO;
			caption.clear();
		} else if (ext == "jpg" || ext == "png" || ext == "jpeg") {
			image.load(path);
			sendPixels(image);
			mode = IMAGE;
			caption.clear();
		}
	}
	void keyPressed(int key) {
		if (key == ' ') {
			mode = WEBCAM;
		}
	}
	template <typename T>
	void drawInternal(T& target) {
		float aspect = width / height;
		float aspect_t = target.getWidth() / target.getHeight();
		if (aspect < aspect_t) {
			float scale = width / target.getWidth();
			target.draw(width * 0.5, height * 0.5, width, target.getHeight() * scale);
		} else {
			float scale = height / target.getHeight();
			target.draw(width * 0.5, height * 0.5, target.getWidth() * scale, height);
		}
	}
	template <typename T>
	void sendPixels(T& target) {
		ofPixels pix = target.getPixels();
		pix.resize(send_image_width, target.getHeight() * target.getHeight() / send_image_width);
		ofBuffer buf;
		ofSaveImage(pix, buf);
		pub.send(buf);
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