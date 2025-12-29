# swlib

C++23 기반의 모던 유틸리티 라이브러리

그냥 제가 코딩하다가 자주 사용하는 유틸리티를 모았습니다.

## 주요 기능
- **Types**: 고정 폭 정수 타입 (`i32`, `u64`, `usize` 등) 및 플랫폼 감지 매크로
- **Meta**: 컴파일 타임 타입 ID 및 이름 추출 (`type_id`, `type_name`)
- **Tuple**: 튜플 평탄화 및 타입 언팩 도구
- **Function**: SBO(Small Buffer Optimization)가 적용된 `sw::function`
- **Utility**: FNV-1a 컴파일 타임 해시, 메모리 정렬 유틸리티

## 요구 사항
- **C++23** 호환 컴파일러 (MSVC, GCC 13+, Clang 16+)
- **CMake 3.28+**

## 빌드 및 테스트
```bash
cmake -B build
cmake --build build
cd build && ctest
```

## 사용 방법 (CMake)
```cmake
add_subdirectory(swlib)
target_link_libraries(your_target PRIVATE swlib)
```
