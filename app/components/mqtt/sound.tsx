import { useRef } from "react";
import { Audio } from "expo-av";
import AsyncStorage from "@react-native-async-storage/async-storage";

export default function useMQTTAlert() {
  const soundRef = useRef<Audio.Sound | null>(null);

  /* ---------------- LOAD USER SOUND ---------------- */

  const loadSelectedSound = async () => {
    const settings = await AsyncStorage.getItem("user_settings");

    if (!settings) return "nature";

    const parsed = JSON.parse(settings);
    return parsed.selectedSound;
  };

  /* ---------------- SOUND FILE SELECTOR ---------------- */

  const getSoundFile = (sound: string) => {
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

  /* ---------------- PLAY SOUND ---------------- */

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

      await soundRef.current.playAsync();
    } catch (err) {
      console.log("Sound play error:", err);
    }
  };

  /* ---------------- STOP SOUND ---------------- */

  const stopSound = async () => {
    try {
      if (soundRef.current) {
        await soundRef.current.stopAsync();
      }
    } catch (err) {
      console.log("Sound stop error:", err);
    }
  };

  /* ---------------- ALERT CONTROLLER ---------------- */

  const triggerAlert = async (stressLevel: number) => {

    if (stressLevel >= 2) {
      console.log("⚠️ High Stress Alert");
      await playSound();
    } else {
      await stopSound();
    }

  };

  return triggerAlert;
}