# 컴파일러 설정
CC = clang

# 컴파일러 플래그 (옵션)
CFLAGS = -Wall -g

# Homebrew 경로 (Makefile 내에서 셸 명령어를 사용할 수 있음)
BREW_PREFIX = $(shell brew --prefix)

# 헤더 파일 경로 및 라이브러리 경로
INCLUDE_DIRS = -Iinclude -I$(BREW_PREFIX)/include
LIB_DIRS = -L$(BREW_PREFIX)/lib

# 링크할 라이브러리
LIBS = -lcurl -ljansson

# 소스 파일 및 오브젝트 파일
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, bin/%.o, $(SRCS))

# 최종 실행 파일 이름 <-- 이 부분을 수정했습니다!
TARGET = bin/stockAnalyzer

# 기본 규칙: 'make' 라고만 입력하면 실행되는 부분
all: $(TARGET)

# 실행 파일을 만드는 규칙
$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)
	@echo "Build complete. Executable is at $(TARGET)"

# .c 파일을 .o 오브젝트 파일로 컴파일하는 규칙
bin/%.o: src/%.c
	@mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# 이전 컴파일 결과물 삭제
clean:
	rm -rf bin