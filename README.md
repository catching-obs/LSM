# Hybrid Media Engine

C++/Rust 하이브리드 멀티미디어 처리 엔진

## 프로젝트 구조

```
project-root/
│
├── CMakeLists.txt               # C++ 전체 빌드 관리
├── Cargo.toml                   # Rust 워크스페이스
├── README.md
├── docs/                        # 아키텍처/기능 문서
│
├── core/                        # C++ 핵심 엔진
│   └── utils/                   # 공통 유틸리티
│       ├── logger.h/cpp         # 로깅 시스템
│       └── memory_pool.h/cpp    # 메모리 풀
│
├── rust-modules/                # Rust 모듈
│   ├── audio-dsp/               # 오디오 믹싱 엔진
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs           # FFI 인터페이스
│   │       └── mixer.rs         # 오디오 믹서 로직
│   │
│   └── video-encoder/           # 비디오 인코더
│       ├── Cargo.toml
│       └── src/
│           ├── lib.rs           # FFI 인터페이스
│           └── pipeline.rs      # 인코딩 파이프라인
│
├── bindings/                    # 양방향 FFI 인터페이스
│   ├── cpp_to_rust/             # C++ → Rust 호출
│   │   ├── audio_ffi.h          # 오디오 FFI 헤더
│   │   └── video_ffi.h          # 비디오 FFI 헤더
│   └── rust_to_cpp/             # Rust → C++ 호출
│       └── lib.rs               # C++ 콜백 인터페이스
│
├── app/                         # 최종 실행 앱
│   ├── CMakeLists.txt
│   └── main.cpp                 # 데모 애플리케이션
│
└── tests/
    ├── cpp/                     # C++ 테스트
    │   ├── CMakeLists.txt
    │   └── test_core.cpp
    └── rust/                    # Rust 테스트 (cargo test)
```

## 주요 기능

### C++ Core
- **Logger**: 타임스탬프 기반 로깅 시스템
- **MemoryPool**: 고정 크기 메모리 풀 관리

### Rust Modules

#### Audio DSP
- 멀티채널 오디오 믹싱
- 채널별 볼륨 조절
- SIMD 최적화 준비

#### Video Encoder
- 비동기 인코딩 파이프라인
- 멀티스레드 프레임 처리
- FFI를 통한 C++ 통합

## 빌드 방법

### 필수 요구사항
- CMake 3.20+
- C++17 호환 컴파일러
- Rust 1.70+ (2021 edition)
- Cargo

### 빌드 단계

1. **Rust 모듈 빌드**
```bash
cd rust-modules
cargo build --release
cd ..
```

2. **C++ 프로젝트 빌드**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

3. **테스트 실행**
```bash
# Rust 테스트
cd rust-modules
cargo test

# C++ 테스트
cd build
ctest
```

4. **애플리케이션 실행**
```bash
cd build/app
./media_app
```

## 사용 예제

### 오디오 믹싱
```cpp
#include "audio_ffi.h"

// 2채널 믹서 생성
AudioMixer* mixer = audio_mixer_create(2);

// 오디오 믹싱
const float* inputs[2] = {channel1, channel2};
audio_mixer_mix(mixer, inputs, output, frame_count);

// 볼륨 조절
audio_mixer_set_channel_volume(mixer, 0, 0.5f);

// 정리
audio_mixer_destroy(mixer);
```

### 비디오 인코딩
```cpp
#include "video_ffi.h"

// 인코더 설정
CEncoderConfig config = {1920, 1080, 30, 5000000};
EncoderPipeline* encoder = encoder_create(config);

// 인코딩 시작
encoder_start(encoder);

// 프레임 제출
encoder_submit_frame(encoder, frame_data, frame_size, timestamp);

// 정리
encoder_stop(encoder);
encoder_destroy(encoder);
```

## 아키텍처 특징

### C++ ↔ Rust FFI
- **C ABI**: 안정적인 바이너리 인터페이스
- **Opaque Pointers**: 타입 안전성 유지
- **Zero-copy**: 포인터 전달로 성능 최적화

### 모듈 분리
- **Audio DSP**: Pure logic, 최소 의존성
- **Video Encoder**: 비동기 처리, crossbeam 활용
- **Core Utils**: C++ 공통 유틸리티

### 빌드 시스템
- **CMake**: C++ 및 통합 빌드
- **Cargo**: Rust 모듈 빌드
- 정적 라이브러리 링킹 (.a/.lib)

## 라이선스

MIT License

## 기여

이슈 및 PR 환영합니다!

## 향후 계획

- [ ] FFmpeg 실제 통합
- [ ] SIMD 최적화 (audio-dsp)
- [ ] GPU 가속 (video-encoder)
- [ ] 더 많은 포맷 지원
- [ ] 벤치마크 스위트
- [ ] 문서 확장
