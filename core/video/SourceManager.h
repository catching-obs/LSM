#ifndef SOURCE_MANAGER_H
#define SOURCE_MANAGER_H

#include "VideoSource.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

class SourceManager {
public:
    static SourceManager& getInstance();

    // Register a source and take ownership
    void addSource(std::shared_ptr<VideoSource> source);
    
    // Remove source by name
    void removeSource(const std::string& name);
    
    // Get source by name
    std::shared_ptr<VideoSource> getSource(const std::string& name);

    // List all active sources
    std::vector<std::string> getSourceNames() const;

private:
    SourceManager() = default;
    ~SourceManager() = default;
    SourceManager(const SourceManager&) = delete;
    SourceManager& operator=(const SourceManager&) = delete;

    std::map<std::string, std::shared_ptr<VideoSource>> sources_;
};

#endif // SOURCE_MANAGER_H
