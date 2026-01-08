#ifndef STREAM_CONTROLLER_H
#define STREAM_CONTROLLER_H

#include <string>
#include <atomic>

class StreamController {
public:
    enum class State {
        IDLE,
        INITIALIZING,
        STREAMING,
        ERROR,
        RECOVERING
    };

    StreamController();
    ~StreamController();

    bool startStreaming(const std::string& url);
    void stopStreaming();
    
    State getState() const;
    void updateNetworkStats(); // Placeholder for network adaptation logic

private:
    std::atomic<State> currentState_;
    std::string currentUrl_;
};

#endif // STREAM_CONTROLLER_H
