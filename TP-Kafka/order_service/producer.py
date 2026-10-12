import json, uuid
from datetime import datetime, timezone
from confluent_kafka import Producer

producer = Producer({"bootstrap.servers": "localhost:9092",
                     "enable.idempotence": True, "acks": "all"})

def publish_order_event(order_id, event_type, data):
    event = {
        "eventId": str(uuid.uuid4()),
        "orderId": order_id,
        "type": event_type,
        "occurredAt": datetime.now(timezone.utc).isoformat(),
        "data": data,
    }
    producer.produce("order-events", key=order_id, value=json.dumps(event))
    producer.flush()
    print(f"[ORDER] Publicado {event_type} para {order_id}")

if __name__ == "__main__":
    publish_order_event("ORD-1001", "ORDER_CREATED", {"total": 59.90})
    publish_order_event("ORD-1001", "ORDER_CONFIRMED", {"customerEmail": "ana@mail.com"})
    publish_order_event("ORD-1001", "ORDER_SHIPPED", {"carrier": "DHL"})