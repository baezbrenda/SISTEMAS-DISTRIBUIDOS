# TP Algoritmo de Cristian

Sincronización de relojes con el método de Cristian, usando UDP.

## Compilar
```
make
```

## Ejecutar
Servidor (el desfase en ms es opcional y simula un reloj adelantado/atrasado):
```
./servidor_tiempo 5000 1500
```
Cliente (IP, puerto y cantidad de muestras opcional):
```
./cliente 127.0.0.1 5000 5
```

## Cómo funciona
1. El cliente anota T0 y envía una petición al servidor.
2. El servidor responde con su hora `Ts`.
3. El cliente anota T1 al recibir la respuesta y calcula `RTT = T1 - T0`.
4. Hora estimada del servidor: `Ts + RTT/2`.
5. Offset = `(Ts + RTT/2) - hora_local`. La precisión es ±RTT/2.
6. Se toman varias muestras y se usa la de menor RTT, que tiene menos error.

El cliente no modifica el reloj del sistema (requeriría root): muestra el offset y la hora sincronizada.