import json
from confluent_kafka import Consumer

c = Consumer({"bootstrap.servers": "localhost:9092",
              "group.id": "notification-service",
              "auto.offset.reset": "earliest",
              "enable.auto.commit": False})
c.subscribe(["order-events"])

MESSAGES = {"ORDER_CREATED": "Hemos recibido tu pedido",
            "ORDER_CONFIRMED": "Tu pedido fue confirmado",
            "ORDER_SHIPPED": "¡Tu pedido va en camino!"}

print("[NOTIF] Esperando eventos...")
while True:
    msg = c.poll(1.0)
    if msg is None or msg.error():
        continue
    event = json.loads(msg.value())
    print(f"[NOTIF] {event['orderId']}: {MESSAGES[event['type']]}")
    c.commit(msg)