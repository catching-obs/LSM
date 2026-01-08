#include "SourceManager.h"
#include <iostream>

SourceManager& SourceManager::getInstance() {
    static SourceManager instance;
    return instance;
}

void SourceManager::addSource(std::shared_ptr<VideoSource> source) {
    if (!source) return;
    std::string name = source->getName();
    sources_[name] = source;
    std::cout << "Source added: " << name << std::endl;
}

void SourceManager::removeSource(const std::string& name) {
    auto it = sources_.find(name);
    if (it != sources_.end()) {
        it->second->stop(); // Ensure it's stopped
        sources_.erase(it);
        std::cout << "Source removed: " << name << std::endl;
    }
}

std::shared_ptr<VideoSource> SourceManager::getSource(const std::string& name) {
    auto it = sources_.find(name);
    if (it != sources_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> SourceManager::getSourceNames() const {
    std::vector<std::string> names;
    for (const auto& pair : sources_) {
        names.push_back(pair.first);
    }
    return names;
}
