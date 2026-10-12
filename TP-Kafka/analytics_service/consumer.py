import json
from collections import Counter
from confluent_kafka import Consumer

c = Consumer({"bootstrap.servers": "localhost:9092",
              "group.id": "analytics-service",
              "auto.offset.reset": "earliest"})
c.subscribe(["order-events"])

counts = Counter()
print("[ANALYTICS] Esperando eventos...")
while True:
    msg = c.poll(1.0)
    if msg is None or msg.error():
        continue
    event = json.loads(msg.value())
    counts[event["type"]] += 1
    print(f"[ANALYTICS] {dict(counts)}")