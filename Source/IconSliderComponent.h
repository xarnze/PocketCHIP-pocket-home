#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class IconSliderComponent : public Component {
public:
  ScopedPointer<Slider> slider;
  ScopedPointer<DrawableButton> iconLow, iconHi;

  StretchableLayoutManager sliderLayout;

  IconSliderComponent(const char *loData, const char *hiData);
  ~IconSliderComponent();

  void paint(Graphics &);
  void resized();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconSliderComponent)
};