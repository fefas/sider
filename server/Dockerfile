FROM debian:bookworm-slim

WORKDIR /var/www/sider-server

RUN apt update && \
    apt install -y \
        build-essential \
        cmake \
        libboost-all-dev \
        libspdlog-dev \
        libfmt-dev \
        libgtest-dev && \
    apt clean

COPY . .

 RUN cmake -S . -B build/ && \
     cmake --build build/
