# 컴파일러 설정
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 $(shell pkg-config --cflags openssl)
LDFLAGS = $(shell pkg-config --libs openssl)

# 실행 파일 및 소스 파일 설정
TARGET = mini_drm
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# 빌드 규칙
all: $(TARGET)

# 오브젝트 파일 생성 규칙
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 최종 실행 파일 빌드
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# 실행
run: $(TARGET)
	./$(TARGET)

# 파일 정리 (컴파일된 파일만 삭제)
clean:
	rm -f $(OBJ)

# 전체 정리 (실행 파일 및 생성된 파일까지 삭제)
fclean: clean
	rm -f $(TARGET) decrypted.pdf encrypted.enc key.bin iv.bin

# 다시 빌드 (완전 초기화 후 빌드)
re: fclean all
