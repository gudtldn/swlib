# 테스트 실행 파일 생성
add_executable(sw_test main.cpp)

# swlib 라이브러리 링크
target_link_libraries(sw_test PRIVATE swlib)

# CMake 테스트 시스템에 등록 (ctest 명령어로 실행 가능)
add_test(NAME RunsCorrectly COMMAND sw_test)
