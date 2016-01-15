#include "ofMain.h"
#include "ofxVoronoi.h"
#include "Camera2d.h"

template <class T>
vector<T> loadTsne(string filename) {
    vector<T> tsne;
    ofBuffer tsneBuffer = ofBufferFromFile(filename);
    for(auto& line : tsneBuffer.getLines()) {
        if(line.size()) {
            T x;
            stringstream(line) >> x;
            tsne.emplace_back(x);
        }
    }
    return tsne;
}

vector<string> loadWords(string filename) {
    vector<string> words;
    ofBuffer wordsFile = ofBufferFromFile(filename);
    for(auto& line : wordsFile.getLines()) {
        if(line.size()) {
            words.emplace_back(line);
        }
    }
    return words;
}

void addCellWireframe(ofMesh& mesh, const ofxVoronoiCell& cell, const ofColor& color) {
    mesh.setMode(OF_PRIMITIVE_LINES);
    vector<ofPoint>::const_iterator itr = cell.pts.begin();
    while(true) {
        mesh.addColor(color);
        mesh.addVertex(*itr);
        itr++;
        mesh.addColor(color);
        if(itr == cell.pts.end()) {
            mesh.addVertex(cell.pts.front());
            break;
        } else {
            mesh.addVertex(*itr);
        }
    }
}

void addCellFill(ofMesh& mesh, const ofxVoronoiCell& cell, const ofColor& color) {
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    const ofPoint& first = cell.pts.front();
    vector<ofPoint>::const_iterator itr = cell.pts.begin();
    while(true) {
        mesh.addColor(color);
        mesh.addVertex(first);
        mesh.addColor(color);
        mesh.addVertex(*itr);
        itr++;
        mesh.addColor(color);
        if(itr == cell.pts.end()) {
            mesh.addVertex(cell.pts.front());
            break;
        } else {
            mesh.addVertex(*itr);
        }
    }
}

class ofApp : public ofBaseApp {
public:
    int res = 1024;
    int nearestIndex = 0;
    
    Camera2d cam;
    ofTrueTypeFont ttf;
    ofxVoronoi voronoi;
    ofVboMesh meshPoints, meshWireframe, meshFill;
    vector<string> words;
    float pointSize;
    
    void setup() {
        ofBackground(255);
        pointSize = 6;
        reset();
        cam.setup();
        cam.setPosition(res/2, res/2);
        ttf.load(OF_TTF_SANS, 16);
    }
    void reset() {
        meshPoints = ofVboMesh();
        words = loadWords("words");
        string perplexity = "10";
        vector<ofVec2f> tsne2d = loadTsne<ofVec2f>(perplexity + ".2d.tsne");
        vector<ofVec3f> tsne3d = loadTsne<ofVec3f>(perplexity + ".3d.tsne");
        int n = tsne2d.size();
        for(int i = 0; i < n; i++) {
            ofVec2f& cur2d = tsne2d[i];
            ofVec3f& cur3d = tsne3d[i];
            meshPoints.addVertex(cur2d * res);
            meshPoints.addColor(ofFloatColor(cur3d[0], cur3d[1], cur3d[2]));
        }
        meshPoints.setMode(OF_PRIMITIVE_POINTS);
        
        float pad = res / 8;
        ofRectangle bounds(-pad, -pad, res+pad*2, res+pad*2);
        voronoi.setBounds(bounds);
        voronoi.setPoints(meshPoints.getVertices());
        voronoi.generate();
        
        updateMesh();
    }
    void update() {
    }
    void updateMesh() {
        meshWireframe = ofVboMesh();
        meshFill = ofVboMesh();
        vector <ofxVoronoiCell> cells = voronoi.getCells();
        for(int i=0; i<cells.size(); i++) {
            addCellFill(meshFill, cells[i], meshPoints.getColor(i));
            addCellWireframe(meshWireframe, cells[i], meshPoints.getColor(i));
            meshPoints.setVertex(i, cells[i].pt);
        }
    }
    void draw() {
        glPointSize(pointSize);
        
        cam.begin();
        
        if(ofGetKeyPressed('w')) {
            meshWireframe.enableColors();
            meshWireframe.draw();
            meshPoints.enableColors();
            meshPoints.draw();
        } else {
            meshFill.draw();
            
            ofSetColor(255, 32);
            meshWireframe.disableColors();
            meshWireframe.draw();
            
            ofSetColor(255, 128);
            meshPoints.disableColors();
            meshPoints.draw();
        }
        
        cam.end();
        
        
        // it would be better to only compute mouseWorld once
        // then compare to all the meshPoints
        // but for some reason that doesn't work correctly
        float nearestDistance = numeric_limits<float>::infinity();
        ofVec2f mouseScreen(mouseX, mouseY);
        for(int i = 0; i < words.size(); i++) {
            ofVec3f position = cam.worldToScreen(meshPoints.getVertex(i));
            // ofGetHeight() - position.y is a hack to deal with Camera2d flipping
            position.y = ofGetHeight() - position.y;
            float distance = mouseScreen.distance(position);
            if(distance < nearestDistance) {
                nearestIndex = i;
                nearestDistance = distance;
            }
        }
        if(nearestDistance > 100) {
            nearestIndex = -1;
        }
        
        for(int i = 0; i < words.size(); i++) {
            string& word = words[i];
            ofVec3f position = cam.worldToScreen(meshPoints.getVertex(i));
            // ofGetHeight() - position.y is a hack to deal with Camera2d flipping
            position.y = ofGetHeight() - position.y;
            ofRectangle rect = ttf.getStringBoundingBox(word, 0, 0);
            ofVec2f size(rect.getWidth(), rect.getHeight());
            bool picked = (i == nearestIndex);
            ofPushMatrix();
            ofTranslate(position - size / 2);
            ofSetColor(picked ? 0 : 255, 128);
            ofDrawRectangle(rect);
            ofSetColor(picked ? 255 : 0);
            ttf.drawString(word, 0, 0);
            ofPopMatrix();
        }
    }
    void keyPressed(int key) {
        if(key == ' ') {
            voronoi.relax();
            updateMesh();
        }
        if(key == 'r') {
            reset();
        }
        if(key == 'f') {
            ofToggleFullscreen();
        }
        if(key == OF_KEY_UP) {
            pointSize+=.25;
        }
        if(key == OF_KEY_DOWN) {
            pointSize-=.25;
        }
        pointSize = ofClamp(pointSize, 0, 10);
    }
};
int main() {
    ofSetupOpenGL(800, 800, OF_WINDOW);
    ofRunApp(new ofApp());
}
