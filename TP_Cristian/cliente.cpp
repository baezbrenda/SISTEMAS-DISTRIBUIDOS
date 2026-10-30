#include <arpa/inet.h>
#include <endian.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std::chrono;

static int64_t reloj_us() {  // reloj de pared (el que se quiere sincronizar)
    return duration_cast<microseconds>(
               system_clock::now().time_since_epoch()).count();
}

static std::string formatear(int64_t us) {
    time_t seg = us / 1000000;
    int resto = static_cast<int>(us % 1000000);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&seg));
    char out[48];
    std::snprintf(out, sizeof(out), "%s.%06d", buf, resto);
    return out;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <ip_servidor> <puerto> [num_muestras]\n";
        return 1;
    }
    const char* ip = argv[1];
    int puerto = std::atoi(argv[2]);
    int muestras = (argc >= 4) ? std::atoi(argv[3]) : 5;
    if (muestras < 1) muestras = 1;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    timeval tv{2, 0};  // timeout de 2 s por si se pierde un paquete
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(puerto);
    if (inet_pton(AF_INET, ip, &srv.sin_addr) != 1) {
        std::cerr << "IP invalida: " << ip << "\n";
        return 1;
    }

    bool hay_mejor = false;
    int64_t mejor_rtt = 0, mejor_offset = 0, mejor_ts = 0;

    std::cout << std::fixed << std::setprecision(3);
    for (int i = 1; i <= muestras; ++i) {
        char peticion = 'T';

        auto t0_mono = steady_clock::now();  // reloj monotonico: mide el RTT
        sendto(sock, &peticion, 1, 0,
               reinterpret_cast<sockaddr*>(&srv), sizeof(srv));

        uint64_t red;
        ssize_t n = recvfrom(sock, &red, sizeof(red), 0, nullptr, nullptr);
        auto t1_mono = steady_clock::now();
        int64_t t1_local = reloj_us();

        if (n != sizeof(red)) {
            std::cout << "Muestra " << i << ": sin respuesta (timeout)\n";
            continue;
        }

        int64_t ts = static_cast<int64_t>(be64toh(red));
        int64_t rtt = duration_cast<microseconds>(t1_mono - t0_mono).count();
        int64_t estimada = ts + rtt / 2;
        int64_t offset = estimada - t1_local;

        std::cout << "Muestra " << i << ": RTT = " << rtt / 1000.0
                  << " ms, offset = " << offset / 1000.0 << " ms\n";

        if (!hay_mejor || rtt < mejor_rtt) {
            hay_mejor = true;
            mejor_rtt = rtt;
            mejor_offset = offset;
            mejor_ts = ts;
        }
    }
    close(sock);

    if (!hay_mejor) {
        std::cerr << "No se obtuvo ninguna respuesta del servidor.\n";
        return 1;
    }

    int64_t local = reloj_us();
    std::cout << "\n--- Resultado (muestra con menor RTT) ---\n"
              << "RTT minimo:           " << mejor_rtt / 1000.0 << " ms\n"
              << "Precision (+/-):      " << mejor_rtt / 2000.0 << " ms\n"
              << "Offset estimado:      " << mejor_offset / 1000.0 << " ms\n"
              << "Hora local:           " << formatear(local) << "\n"
              << "Hora sincronizada:    " << formatear(local + mejor_offset) << "\n";

    (void)mejor_ts;
    return 0;
}