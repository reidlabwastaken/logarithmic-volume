#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

float k = 9.0f;
float volumeToSlider(float n) { return std::log1p(n * k) / std::log1p(k); }
float sliderToVolume(float n) { return std::expm1(n * std::log1p(k)) / k; }

template<typename Parent, void (FMODAudioEngine::*Setter)(float)>
void handleSliderChange(CCObject* sender, void (Parent::*fallback)(CCObject*)) {
    SliderThumb* slider = typeinfo_cast<SliderThumb*>(sender);
    if (!slider) { return (Parent{}.*fallback)(sender); }
    (FMODAudioEngine::get()->*Setter)(sliderToVolume(slider->getValue()));
}

class $modify(OptionsLayer) {
    $override void customSetup() {
        OptionsLayer::customSetup();
        FMODAudioEngine* fmod = FMODAudioEngine::get();
        m_musicSlider->setValue(volumeToSlider(fmod->getBackgroundMusicVolume()));
        m_sfxSlider->setValue(volumeToSlider(fmod->getEffectsVolume()));
    }

    $override void musicSliderChanged(CCObject* sender) { handleSliderChange<OptionsLayer, &FMODAudioEngine::setBackgroundMusicVolume>(sender, &OptionsLayer::sfxSliderChanged); }
    $override void sfxSliderChanged(CCObject* sender) { handleSliderChange<OptionsLayer, &FMODAudioEngine::setEffectsVolume>(sender, &OptionsLayer::sfxSliderChanged); }
};

class $modify(PauseLayer) {
    $override void customSetup() {
        PauseLayer::customSetup();
        Slider* musicSlider = typeinfo_cast<Slider*>(getChildByID("music-slider"));
        Slider* sfxSlider = typeinfo_cast<Slider*>(getChildByID("sfx-slider"));
        FMODAudioEngine* fmod = FMODAudioEngine::get();
        if (musicSlider) { musicSlider->setValue(volumeToSlider(fmod->getBackgroundMusicVolume())); }
        if (sfxSlider) { sfxSlider->setValue(volumeToSlider(fmod->getEffectsVolume())); }
    }

    $override void musicSliderChanged(CCObject* sender) { handleSliderChange<PauseLayer, &FMODAudioEngine::setBackgroundMusicVolume>(sender, &PauseLayer::sfxSliderChanged); }
    $override void sfxSliderChanged(CCObject* sender) { handleSliderChange<PauseLayer, &FMODAudioEngine::setEffectsVolume>(sender, &PauseLayer::sfxSliderChanged); }
};
