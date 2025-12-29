# 테스트 대상 소스 파일들 탐색 (test_*.cpp)
file(GLOB TEST_SOURCES "test_*.cpp")

foreach(test_source ${TEST_SOURCES})
    # 파일 이름에서 확장자 제거하여 타겟 이름 생성 (예: test_concepts)
    get_filename_component(test_name ${test_source} NAME_WE)

    # 실행 파일 생성
    add_executable(${test_name} ${test_source})

    # 컴파일러 옵션 설정
    if (MSVC)
        target_compile_options(${test_name} PRIVATE
                /Zc:preprocessor # MSVC 전처리기 최신 표준 기능을 활성화
                /Zc:__cplusplus  # __cplusplus 매크로가 올바른 버전을 반환하도록 수정
                /utf-8           # UTF-8 인코딩 사용
        )
    else ()
        target_compile_options(${test_name} PRIVATE
                -finput-charset=UTF-8
                -fexec-charset=UTF-8
        )
    endif ()

    # 라이브러리 링크 및 헤더 경로 포함
    target_link_libraries(${test_name} PRIVATE swlib)
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # CTest에 테스트 등록
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
