# TP - Eventos con Kafka

Cada cambio de estado de un pedido (creado, confirmado, enviado) se publica en el topic `order-events` de Kafka. Dos servicios lo consumen: **NotificationService** (muestra un aviso) y **AnalyticsService** (cuenta los eventos). Cada uno tiene su propio `group.id`, así que los dos reciben todos los eventos.

**Tecnologías:** Kafka, Docker Compose, Python.

## Kafka vs RabbitMQ

- **Mensajes:** Kafka los guarda aunque se consuman; RabbitMQ los borra al consumirse.
- **Varios servicios:** en Kafka cada uno lee todo con su `group.id`; en RabbitMQ hay que crear una cola por servicio.
- **Eventos viejos:** Kafka permite releerlos; RabbitMQ no.
- **Complejidad:** Kafka es más complejo; RabbitMQ es más simple.

**Conclusión:** Kafka encaja mejor porque varios servicios necesitan los mismos eventos y Analytics aprovecha el histórico. Además, se pueden agregar servicios nuevos sin tocar el productor.