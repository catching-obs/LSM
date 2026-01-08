# 김승우 담당 모듈 세부 설계서

*(영상 입력 · 송출 · 상태 관리 파트)*

## 1. 담당 범위 요약

김승우는 본 프로젝트에서 **실시간 영상 입력과 송출 파이프라인의 핵심 모듈**을 담당한다.
해당 영역은 엔진 전체의 안정성과 실시간성을 직접적으로 좌우하며, 아래 기능을 포함한다.

* 카메라 입력 처리 (Video Capture)
* 화면 캡처 입력 처리 (Display / Window Capture)
* 영상 송출 시스템 (WebRTC / RTMP / SRT)
* 송출·녹화 상태 관리 및 에러 처리
* 엔진 내부 VideoSource 통합 인터페이스 구현

---

## 2. 관련 디렉터리 구조 (김승우 담당)

```text
/project-root
├── core/
│   ├── video/
│   │   ├── VideoSource.h
│   │   ├── VideoFrame.h
│   │   ├── CameraSource.cpp
│   │   ├── ScreenSource.cpp
│   │   └── SourceManager.cpp
│   │
│   ├── streaming/
│   │   ├── StreamController.h
│   │   ├── WebRTCStreamer.cpp
│   │   ├── RTMPStreamer.cpp
│   │   └── StreamStateMachine.cpp
│   │
│   └── device/
│       ├── CameraDevice.cpp
│       └── AudioInputDevice.cpp
```

---

## 3. 영상 입력 시스템 설계

### 3.1 VideoSource 공통 인터페이스

모든 영상 입력은 동일한 추상 인터페이스를 구현한다.

```cpp
class VideoSource {
public:
    virtual bool start() = 0;
    virtual bool getFrame(VideoFrame& frame) = 0;
    virtual void stop() = 0;
    virtual ~VideoSource() = default;
};
```

이를 통해:

* 카메라
* 화면 캡처
* 윈도우 캡처

가 **동일한 엔진 파이프라인**에서 처리된다.

---

### 3.2 CameraSource (카메라 입력)

**역할**

* OS API 기반 카메라 장치 접근
* 프레임 수신 및 VideoFrame 변환
* 프레임 타임스탬프 부여

**주요 책임**

* 카메라 장치 열거 및 선택
* 해상도, FPS 협상
* 장치 오류 발생 시 안전한 재시작

```cpp
class CameraSource : public VideoSource {
public:
    bool start() override;
    bool getFrame(VideoFrame& frame) override;
    void stop() override;
};
```

---

### 3.3 ScreenSource (화면 / 창 캡처)

**역할**

* 디스플레이 전체 또는 특정 윈도우 캡처
* GPU 또는 CPU 캡처 방식 선택 가능 구조

**특징**

* 캡처 방식 차이를 엔진 상위 계층에 노출하지 않음
* 고정 FPS 보장 로직 포함

---

## 4. 송출 시스템 설계

### 4.1 StreamController

송출 시스템의 상위 제어 모듈로, UI 및 엔진과 직접 연결된다.

**책임**

* 송출 시작 / 중지 제어
* 송출 대상(WebRTC, RTMP, SRT) 선택
* 네트워크 상태 기반 파라미터 조정 요청

```cpp
class StreamController {
public:
    bool startStreaming();
    void stopStreaming();
    void updateNetworkStats();
};
```

---

### 4.2 WebRTC / RTMP 송출 모듈

**역할**

* 인코딩된 비디오 프레임 수신
* 프로토콜별 송출 파이프라인 관리
* 네트워크 지연 및 패킷 손실 대응

**설계 원칙**

* 인코딩 파이프라인과 송출 파이프라인 분리
* 프로토콜별 모듈 독립 유지

---

## 5. 송출·녹화 상태 관리 및 에러 처리

### 5.1 상태 머신 설계

송출 및 녹화는 명확한 상태 전이를 따른다.

```
IDLE
 → INITIALIZING
 → STREAMING
 → ERROR
 → RECOVERING
 → IDLE
```

**장점**

* 예측 가능한 동작
* UI 상태와의 정합성 유지
* 오류 발생 시 자동 복구 가능

---

### 5.2 에러 처리 정책

* 네트워크 오류 → 자동 재시도
* 장치 오류 → 입력 소스 재연결
* 치명적 오류 → 안전 중단 및 사용자 알림

에러는 모두 **중앙 상태 관리자**를 통해 보고된다.

---

## 6. Rust 모듈과의 연계

김승우 담당 영역은 Rust 모듈과 다음 지점에서 연결된다.

* GPU 렌더러 출력 텍스처 수신
* Rust 인코더에서 생성된 인코딩 결과 수신
* FFI 기반 비동기 데이터 전달

C++은 제어 흐름을 담당하고,
Rust는 고성능 처리 파트를 담당한다.

---

## 7. 처리 흐름 요약

```
[ Camera / Screen ]
        ↓
   VideoSource (C++)
        ↓
   Frame Buffer
        ↓
   GPU Renderer (Rust)
        ↓
   Encoder (Rust)
        ↓
  StreamController (C++)
        ↓
     Network Output
```

---

## 8. 결론

김승우 담당 모듈은 본 프로젝트의 **실시간성·안정성·확장성**을 책임지는 핵심 구성 요소이다.

특히:

* 영상 입력 통합
* 송출 파이프라인 관리
* 상태 기반 오류 처리

를 통해 OBS 수준의 안정적인 영상 처리 흐름을 목표로 한다.
