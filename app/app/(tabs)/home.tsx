import React, { useEffect, useState, useCallback } from "react";
import { StyleSheet, View, Text } from "react-native";
import AsyncStorage from "@react-native-async-storage/async-storage";
import { useFocusEffect } from "@react-navigation/native";

import StressCircle from "@/components/StressCircle";
import InfoCard from "@/components/InfoCard";
import useMQTTAlert from "@/components/mqtt/sound";

export default function HomeScreen() {

  /* ---------------- TYPES ---------------- */

  type StressConfig = {
    colour: string;
    iconBackground: string;
    status: string;
  };

  type StatusConfig = {
    colour: string;
    iconBackground: string;
    status: string;
  };

  /* ---------------- STATE ---------------- */

  const [pattern, setPattern] = useState("pattern1");
  const [index, setIndex] = useState(0);

  /* ---------------- RESET WHEN ENTER SCREEN ---------------- */

  useFocusEffect(
    useCallback(() => {

      const resetPage = async () => {
        try {

          setIndex(0);

          const settings = await AsyncStorage.getItem("user_settings");

          if (settings) {
            const parsed = JSON.parse(settings);

            if (parsed.mockPattern) {
              setPattern(parsed.mockPattern);
            }
          }

        } catch (err) {
          console.log("Settings load error:", err);
        }
      };

      resetPage();

    }, [])
  );

  /* ---------------- MOCK DATA ---------------- */

  const patterns = {

    pattern1: {
      HRV: [0, 40, 42, 38, 50, 55, 47, 60, 58, 45, 52],
      STRESS: [0, 0, 1, 1, 2, 2, 3, 1, 0, 2, 1],
      BPM: [0, 72, 75, 80, 90, 110, 95, 88, 70, 65, 78],
    },

    pattern2: {
      HRV: [0, 35, 30, 28, 25, 22, 20, 18, 15, 12, 10],
      STRESS: [0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3],
      BPM: [0, 80, 85, 90, 95, 100, 110, 115, 120, 125, 130],
    },

    pattern3: {
      HRV: [0, 50, 52, 55, 58, 60, 62, 65, 63, 61, 59],
      STRESS: [0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0],
      BPM: [0, 65, 68, 70, 72, 75, 73, 71, 69, 67, 66],
    },

  };

  const currentPattern = patterns[pattern as keyof typeof patterns];

  const HRV_LIST = currentPattern.HRV;
  const STRESS_LIST = currentPattern.STRESS;
  const BPM_LIST = currentPattern.BPM;

  /* ---------------- LOOP MOCK DATA ---------------- */

  useEffect(() => {

    const interval = setInterval(() => {
      setIndex((prev) => (prev + 1) % HRV_LIST.length);
    }, 10000);

    return () => clearInterval(interval);

  }, [pattern]);

  /* ---------------- CURRENT VALUES ---------------- */

  const HRV = HRV_LIST[index];
  const StressLevel = STRESS_LIST[index];
  const Bpm = BPM_LIST[index];

  /* ---------------- STRESS CONFIG ---------------- */

  const stressConfig: Record<number, StressConfig> = {

    0: {
      colour: "#22C55E",
      iconBackground: "#DCFCE7",
      status: "ผ่อนคลาย",
    },

    1: {
      colour: "#14B8A6",
      iconBackground: "#DDF3F2",
      status: "ปกติ",
    },

    2: {
      colour: "#F59E0B",
      iconBackground: "#FEF3C7",
      status: "มีความเครียด",
    },

    3: {
      colour: "#EF4444",
      iconBackground: "#FEE2E2",
      status: "ความเครียดสูง",
    },

  };

  const stressStatus = stressConfig[StressLevel] || stressConfig[0];

  /* ---------------- BPM CONFIG ---------------- */

  const BPMConfig: Record<number, StatusConfig> = {

    0: {
      colour: "#3B82F6",
      iconBackground: "#DBEAFE",
      status: "ชีพจรต่ำ",
    },

    1: {
      colour: "#22C55E",
      iconBackground: "#DCFCE7",
      status: "ปกติ",
    },

    2: {
      colour: "#F59E0B",
      iconBackground: "#FEF3C7",
      status: "ชีพจรสูง",
    },

    3: {
      colour: "#EF4444",
      iconBackground: "#FEE2E2",
      status: "ชีพจรสูงมาก",
    },

  };

  const getBpmLevel = (bpm: number) => {

    if (bpm < 60) return 0;
    if (bpm <= 100) return 1;
    if (bpm <= 120) return 2;

    return 3;
  };

  const bpmLevel = getBpmLevel(Bpm);
  const bpmStatus = BPMConfig[bpmLevel];

  /* ---------------- ALERT SOUND ---------------- */

  const triggerAlert = useMQTTAlert();

  useEffect(() => {
    triggerAlert(StressLevel);
  }, [StressLevel]);

  /* ---------------- UI ---------------- */

  return (
    <View style={styles.container}>

      <Text style={styles.title}>
        ค่าความเครียด
      </Text>

      <StressCircle value={HRV} />

      <View style={styles.cards}>

        <InfoCard
          icon="heart"
          label="อัตราเต้นหัวใจ"
          value={Bpm}
          unit="BPM"
          colour={bpmStatus.colour}
          iconBackground={bpmStatus.iconBackground}
        />

        <InfoCard
          icon="pulse"
          label="สถานะ"
          value={stressStatus.status}
          colour={stressStatus.colour}
          iconBackground={stressStatus.iconBackground}
        />

      </View>

    </View>
  );
}

/* ---------------- STYLES ---------------- */

const styles = StyleSheet.create({

  container: {
    flex: 1,
    backgroundColor: "#EDEFF0",
    paddingTop: 60,
    alignItems: "center",
  },

  title: {
    fontSize: 26,
    fontWeight: "bold",
    marginBottom: 20,
  },

  cards: {
    width: "85%",
    marginTop: 30,
  },

});