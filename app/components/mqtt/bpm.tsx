import { useEffect, useState } from "react";
import mqtt from "mqtt";

export default function useMQTTBpm() {
  const [stress, setStress] = useState<number>(0);

  useEffect(() => {
    const client = mqtt.connect("ws://iot.cpe.ku.ac.th:9001", {
      username: "b6810503731",
      password: "panparin.r@ku.th",
      reconnectPeriod: 5000,
    });

    client.on("connect", () => {
      console.log("MQTT Connected");

      client.subscribe("b6810503731/bpm", (err) => {
        if (!err) {
          console.log("Subscribed to stress topic");
        }
      });
    });

    client.on("message", (topic, message) => {
      if (topic === "b6810503731/bpm") {
        const value = parseInt(message.toString(),10);
        setStress(value);
      }
    });

    client.on("error", (err) => {
      console.log("MQTT Error:", err);
    });

    return () => {
      client.end();
    };
  }, []);

  return stress;
}