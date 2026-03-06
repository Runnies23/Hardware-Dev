import React from "react";
import { View, Text, StyleSheet } from "react-native";
import Svg, { Circle } from "react-native-svg";

type StressCircleProps = {
  value: number;
};

const StressCircle: React.FC<StressCircleProps> = ({ value }) => {
  type HRVConfig = {
    colour: string;
    label: string;
    };

    const HRV_CONFIG: Record<number, HRVConfig> = {
      0: { colour: "#22C55E", label: "ผ่อนคลาย" },
      1: { colour: "#14B8A6", label: "ปกติ" },
      2: { colour: "#F59E0B", label: "เริ่มเครียด" },
      3: { colour: "#EF4444", label: "เครียดสูง" },
    };
    const getHRVLevel = (hrv: number) => {
      if (hrv > 70) return 0;
      if (hrv > 40) return 1;
      if (hrv > 20) return 2;
      return 3;
    };
  const radius = 90;
  const strokeWidth = 16;
  const circumference = 2 * Math.PI * radius;

  const progress = value / 100;
  const strokeDashoffset = circumference * (1 - progress);

  const level = getHRVLevel(value);
  const config = HRV_CONFIG[level];

  return (
    <View style={styles.container}>
      <Svg width={220} height={220}>
        <Circle
          stroke="#dceeee"
          fill="none"
          cx="110"
          cy="110"
          r={radius}
          strokeWidth={strokeWidth}
        />

        <Circle
          stroke={config.colour}
          fill="none"
          cx="110"
          cy="110"
          r={radius}
          strokeWidth={strokeWidth}
          strokeDasharray={circumference}
          strokeDashoffset={strokeDashoffset}
          strokeLinecap="round"
          rotation="-90"
          origin="110,110"
        />
      </Svg>

      <View style={styles.textContainer}>
        <Text style={[styles.number, { color: config.colour }]}>
          {value}
        </Text>
        <Text style={styles.alert}>{config.label}</Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    justifyContent: "center",
    alignItems: "center",
  },
  textContainer: {
    position: "absolute",
    alignItems: "center",
  },
  number: {
    fontSize: 48,
    fontWeight: "bold",
    color: "#FFC300",
  },
  alert: {
    fontSize: 18,
    color: "#555",
  },
});

export default StressCircle;