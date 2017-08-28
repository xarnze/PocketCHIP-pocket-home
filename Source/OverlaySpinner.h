#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class OverlaySpinner;

class OverlaySpinnerTimer : public Timer {
public:
    OverlaySpinnerTimer(OverlaySpinner *os) {
      overlaySpinner = os;
    };

    void timerCallback();

    OverlaySpinner* overlaySpinner;

    int i = 0;
    int t = 0;
    int timeout = 30 * 1000;
};

class OverlaySpinner : public Component {
public:
    ScopedPointer<ImageComponent> spinnerImage;
    Array<Image> spinnerImages;
    OverlaySpinnerTimer overlaySpinnerTimer;

    OverlaySpinner();
    ~OverlaySpinner();
    
    void setVisible(bool shouldBeVisible);
    void paint(Graphics &) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlaySpinner)
};
