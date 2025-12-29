cmake_minimum_required(VERSION 3.28)
project(swlib
        VERSION 0.1
        LANGUAGES CXX
)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# src 폴더 아래의 모든 .cpp 찾기
file(GLOB_RECURSE SW_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# include 폴더 아래의 모든 .hpp 찾기 (IDE 표시용)
file(GLOB_RECURSE SW_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# 라이브러리 타겟 생성
add_library(swlib STATIC
        ${SW_SOURCES}
        ${SW_HEADERS}
)

# Include 경로 설정
target_include_directories(swlib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# 컴파일러 옵션 설정
if (MSVC)
    target_compile_options(swlib PRIVATE
            /Zc:preprocessor # MSVC 전처리기 최신 표준 기능을 활성화
            /Zc:__cplusplus  # __cplusplus 매크로가 올바른 버전을 반환하도록 수정
            /utf-8           # UTF-8 인코딩 사용
            /W4 /permissive- # 컴파일러 경고 옵션
    )
else ()
    target_compile_options(swlib PRIVATE
            -finput-charset=UTF-8
            -fexec-charset=UTF-8
            -Wall -Wextra -Wpedantic
    )
endif ()

# 라이브러리 테스트 폴더 추가
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(test)
endif()
