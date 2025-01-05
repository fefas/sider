FROM debian:bullseye-slim

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        libboost-all-dev \
        libspdlog-dev \
        libfmt-dev \
        libgtest-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /var/www/sider-server
COPY . .

RUN g++ -std=c++20 \
        src/Clock.cpp \
        src/Command/Commands/*.cpp \
        src/Registry/*.cpp \
        src/Registry/Register/*.cpp \
        src/Registry/Register/Registers/*.cpp \
        tests.cpp \
        tests/*.cpp \
        -lgtest \
        -lgtest_main \
        -pthread \
        -o tests/run

RUN g++ -std=c++20 \
        main.cpp \
        src/*.cpp \
        src/Command/Commands/*.cpp \
        src/Registry/*.cpp \
        src/Registry/Register/*.cpp \
        src/Registry/Register/Registers/*.cpp \
        -lgtest \
        -lgtest_main \
        -pthread \
        -lspdlog \
        -lfmt \
        -o main
