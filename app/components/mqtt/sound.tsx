import { useEffect, useRef } from "react";
import mqtt from "mqtt";
import { Audio } from "expo-av";
import AsyncStorage from "@react-native-async-storage/async-storage";

export default function useMQTTAlert() {
  const soundRef = useRef<Audio.Sound | null>(null);

  const loadSelectedSound = async () => {
    const settings = await AsyncStorage.getItem("user_settings");

    if (!settings) return "nature";

    const parsed = JSON.parse(settings);
    return parsed.selectedSound;
  };

  const getSoundFile = (sound: string) => {
    console.log(sound);
    switch (sound) {
      case "nature":
        return require("../sound/nature.mp3");

      case "piano":
        return require("../sound/piano.mp3");

      case "therapy":
        return require("../sound/therapy.mp3");

      case "meditation":
        return require("../sound/meditation.mp3");

      default:
        return require("../sound/therapy.mp3");
    }
  };

  const playSound = async () => {
    try {
      const selectedSound = await loadSelectedSound();
      const file = getSoundFile(selectedSound);

      if (!soundRef.current) {
        const { sound } = await Audio.Sound.createAsync(file, {
          isLooping: true,
        });

        soundRef.current = sound;
      }

      await soundRef.current.replayAsync();
    } catch (err) {
      console.log("Sound play error:", err);
    }
  };

  useEffect(() => {
    const client = mqtt.connect("ws://iot.cpe.ku.ac.th:9001", {
      username: "b6810503731",
      password: "panparin.r@ku.th",
      reconnectPeriod: 5000,
    });

    client.on("connect", () => {
      console.log("Alert MQTT Connected");

      client.subscribe("b6810503731/light", (err) => {
        if (!err) {
          console.log("Subscribed to alert topic");
        }
      });
    });

    client.on("message", async (topic, message) => {
      const msg = message.toString();

      console.log("message:", msg);

      if (topic === "b6810503731/light" && msg === "1") {
        console.log("ALERT TRIGGERED");
        await playSound();
      }

      if (topic === "b6810503731/light" && msg === "0") {
        if (soundRef.current) {
          await soundRef.current.stopAsync();
        }
      }
    });

    client.on("error", (err) => {
      console.log("MQTT Error:", err);
    });

    return () => {
      client.end();

      if (soundRef.current) {
        soundRef.current.unloadAsync();
      }
    };
  }, []);
}