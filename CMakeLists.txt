cmake_minimum_required(VERSION 3.28)
project(swlib
        VERSION 0.1.0
        LANGUAGES CXX
)

# src 폴더 아래의 모든 .cpp 찾기
file(GLOB_RECURSE SW_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# include 폴더 아래의 모든 .hpp 찾기 (IDE 표시용)
file(GLOB_RECURSE SW_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# 라이브러리 타겟 생성
add_library(swlib STATIC
        ${SW_SOURCES}
        ${SW_HEADERS}
)

# C++ 표준 설정
set_target_properties(swlib PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_compile_features(swlib PUBLIC
        cxx_std_23
)

# Include 경로 설정
target_include_directories(swlib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# 컴파일러 옵션 설정
if (MSVC)
    target_compile_options(swlib PRIVATE
            /utf-8           # 소스/실행 파일 인코딩을 UTF-8로 사용
            /permissive-     # MSVC 비표준 확장 끄기
            /Zc:preprocessor # MSVC 전처리기가 표준을 준수하도록 설정
            /Zc:__cplusplus  # 표준 __cplusplus 버전을 사용
            /W4              # 경고 수준을 최고 수준으로 설정
    )
else ()
    target_compile_options(swlib PRIVATE
            -finput-charset=UTF-8 # 소스 파일 인코딩을 UTF-8로 설정
            -fexec-charset=UTF-8  # 실행 파일 문자열 인코딩을 UTF-8로 설정

            -pedantic             # 비표준 문법 경고 (MSVC /permissive- 대응)

            -Wall -Wextra         # 일반적인 모든 경고와 추가적인 경고까지 활성화

    )
endif ()

# 라이브러리 테스트 폴더 추가
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(test)
endif()
