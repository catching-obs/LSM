# 김승우 담당 모듈 기능 명세서

**작성일:** 2026-01-08  
**담당자:** 김승우  
**버전:** 1.0.0

---

## 1. 개요
본 문서는 하이브리드 미디어 엔진 프로젝트에서 **영상 입력(Video Capture), 오디오 입출력(Audio I/O), 장치 관리(Device Management)**를 담당하는 모듈의 구현 기능과 사용법을 기술한다.
해당 모듈은 C++로 작성되었으며, Rust 코어와의 연동을 위한 FFI 바인딩을 제공한다.

---

## 2. 주요 기능

### 2.1 영상 입력 시스템 (Video Input)
*   **CameraSource**: 실제 하드웨어 카메라로부터 프레임을 실시간으로 획득.
    *   **구현 기술**: OpenCV (`cv::VideoCapture`)
    *   **기능**:
        *   자동 장치 인식 및 연결 (기본값: Index 0)
        *   색상 공간 변환 (BGR → RGBA) 자동 처리
        *   타임스탬프(Microseconds) 부여
    *   **출력 데이터**: `VideoFrame` (Width, Height, Raw RGBA Data, Timestamp)

### 2.2 오디오 입출력 시스템 (Audio I/O)
*   **MicrophoneSource**: 마이크로부터 PCM 오디오 데이터를 실시간 캡처.
    *   **구현 기술**: SDL2 Audio
    *   **기능**:
        *   시스템 기본 장치 및 특정 장치 선택 가능
        *   Floating Point (32-bit float), Stereo, 44.1kHz 포맷 표준화
        *   내부 큐(Queue)를 통한 버퍼링 및 스레드 안전성 보장
*   **SpeakerSink**: 오디오 데이터를 스피커로 출력 (Loopback 테스트 및 모니터링용).
    *   **구현 기술**: SDL2 Audio
    *   **기능**:
        *   실시간 오디오 스트림 재생
        *   지연 시간(Latency) 방지를 위한 큐 크기 자동 관리

### 2.3 장치 관리 (Device Management)
*   **장치 검색 (Enumeration)**: 현재 시스템에 연결된 카메라 및 오디오 장치 목록을 반환.
    *   `CameraDeviceManager::getDeviceList()`: 사용 가능한 카메라 인덱스 스캔.
    *   `AudioDeviceManager::getInputDevices()`: SDL2를 통해 시스템 오디오 입력 장치명 조회.
*   **장치 전환 (Switching)**: 런타임 중 소스 객체를 재생성하여 입력 장치를 변경하는 기능 지원.

---

## 3. 디렉터리 구조 및 파일

```text
core/
├── video/
│   ├── CameraSource.h/cpp       # 카메라 캡처 구현 (OpenCV)
│   ├── VideoFrame.h             # 비디오 데이터 구조체
│   └── SourceManager.h/cpp      # (구조적) 소스 관리자
├── audio/
│   ├── MicrophoneSource.h/cpp   # 마이크 캡처 구현 (SDL2)
│   ├── SpeakerSink.h/cpp        # 스피커 재생 구현 (SDL2)
│   └── AudioFrame.h             # 오디오 데이터 구조체
└── streaming/
    └── StreamController.h/cpp   # (초안) 송출 제어기

bindings/cpp_to_rust/
├── camera_ffi.h/cpp             # Rust 연동용 C API (카메라)
└── audio_ffi.h/cpp              # Rust 연동용 C API (오디오)
```

---

## 4. Rust 연동 API (FFI)

Rust(또는 다른 언어)에서 해당 모듈을 제어하기 위해 제공되는 C-compatible API입니다.

### Video API (`camera_ffi.h`)
| 함수명 | 설명 |
|---|---|
| `camera_create(index)` | 카메라 컨텍스트 생성 |
| `camera_start(ctx)` | 캡처 시작 |
| `camera_get_frame(ctx, buf, ...)` | 최신 프레임 데이터 복사 |
| `camera_stop(ctx)` | 캡처 중지 |
| `camera_destroy(ctx)` | 리소스 해제 |

### Audio API (`audio_ffi.h`)
| 함수명 | 설명 |
|---|---|
| `audio_create(device_index)` | 오디오 컨텍스트 생성 |
| `audio_start(ctx)` | 캡처 시작 |
| `audio_get_frame(ctx, buf, ...)` | PCM 데이터 획득 (Float 32bit) |
| `audio_mixer_mix(...)` | 다채널 오디오 믹싱 유틸리티 |

---

## 5. 테스트 및 실행 방법

작성된 기능 검증을 위해 `tests/cpp/` 경로에 실행 가능한 테스트 프로그램이 포함되어 있습니다.

### 빌드 명령어
```bash
mkdir -p build && cd build
cmake ..
make
```

### 테스트 목록
1.  **카메라 화면 테스트** (`test_camera_integration`)
    *   **기능**: 웹캠을 켜고 화면에 윈도우를 띄워 실시간 영상을 보여줌.
    *   **실행**: `./tests/cpp/test_camera_integration` (종료: ESC)
2.  **오디오 루프백 테스트** (`test_audio_loopback`)
    *   **기능**: 마이크로 입력된 소리를 즉시 스피커로 출력 (Echo Test).
    *   **실행**: `./tests/cpp/test_audio_loopback` (종료: Ctrl+C)
3.  **A/V 통합 테스트** (`test_av_integration`)
    *   **기능**: 영상 프레임과 오디오 청크를 동시에 수집하며 FPS/데이터 통계를 로그로 출력.
    *   **실행**: `./tests/cpp/test_av_integration`
4.  **장치 변경 테스트** (`test_device_switching`)
    *   **기능**: 연결된 장치 목록을 출력하고, 장치를 껐다 켜거나 변경하는 시나리오 수행.
    *   **실행**: `./tests/cpp/test_device_switching`

---

## 6. 의존성 (Dependencies)
*   **OpenCV 4.x**: 영상 캡처 및 이미지 처리
*   **SDL2**: 오디오 장치 제어 및 캡처/재생
*   **CMake 3.20+**: 빌드 시스템
