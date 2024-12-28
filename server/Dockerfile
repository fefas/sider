FROM gcc:14.2.0

WORKDIR /var/www/sider-server

COPY . .
RUN g++ server.cpp -o server

ENTRYPOINT ["./server"]
