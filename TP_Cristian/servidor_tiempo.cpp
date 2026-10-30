#include <arpa/inet.h>
#include <endian.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

static int64_t ahora_us() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
               system_clock::now().time_since_epoch()).count();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto> [desfase_ms]\n";
        return 1;
    }
    int puerto = std::atoi(argv[1]);
    int64_t desfase_us = (argc >= 3) ? std::atoll(argv[2]) * 1000 : 0;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(puerto);

    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    std::cout << "Servidor de tiempo escuchando en el puerto " << puerto
              << " (desfase: " << desfase_us / 1000 << " ms)\n";

    while (true) {
        char buf[16];
        sockaddr_in cli{};
        socklen_t len = sizeof(cli);
        ssize_t n = recvfrom(sock, buf, sizeof(buf), 0,
                             reinterpret_cast<sockaddr*>(&cli), &len);
        if (n < 0) { perror("recvfrom"); continue; }

        // Tomar la hora lo mas tarde posible, justo antes de responder
        int64_t t_servidor = ahora_us() + desfase_us;
        uint64_t red = htobe64(static_cast<uint64_t>(t_servidor));

        sendto(sock, &red, sizeof(red), 0,
               reinterpret_cast<sockaddr*>(&cli), len);

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        std::cout << "Peticion de " << ip << ":" << ntohs(cli.sin_port)
                  << " -> respondido " << t_servidor << " us\n";
    }
    close(sock);
    return 0;
}