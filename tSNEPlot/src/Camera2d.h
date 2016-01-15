#pragma once
#include "ofMain.h"

class Camera2d : public ofCamera {
private:
    ofVec2f mouseStart;
    ofVec2f startPosition;
    float zoom;
    float maxZoom = .01;
    float minZoom = 1;
    float zoomSpeed = 1. / 500;
public:
    void setup() {
        setupPerspective(true);
        ofVec3f position = getPosition();
        ofCamera::setPosition(0, 0, position.z);
        zoom = 1;
        ofAddListener(ofEvents().mouseDragged, this, &Camera2d::mouseDragged);
        ofAddListener(ofEvents().mousePressed, this, &Camera2d::mousePressed);
        ofAddListener(ofEvents().mouseScrolled, this, &Camera2d::mouseScrolled);
    }
    void mouseDragged(ofMouseEventArgs& e) {
        ofVec3f position = getPosition();
        position.x = startPosition.x - (e.x - mouseStart.x) * zoom;
        position.y = startPosition.y - (e.y - mouseStart.y) * zoom;
        ofCamera::setPosition(position);
    }
    void mousePressed(ofMouseEventArgs& e) {
        mouseStart = e;
        startPosition = getPosition();
    }
    void mouseScrolled(ofMouseEventArgs& e) {
        zoom += -e.scrollY * zoomSpeed;
        zoom = ofClamp(zoom, maxZoom, minZoom);
        setFov(60 * zoom);
    }
    void setPosition(float x, float y) {
        ofVec3f position = getPosition();
        ofCamera::setPosition(x, y, position.z);
    }
};