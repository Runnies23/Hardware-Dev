import React from "react";
import { View, Text, StyleSheet } from "react-native";
import Svg, { Circle } from "react-native-svg";

type StressCircleProps = {
  value: number;
};

const StressCircle: React.FC<StressCircleProps> = ({ value }) => {
  const radius = 90;
  const strokeWidth = 16;
  const circumference = 2 * Math.PI * radius;

  const progress = value / 100;
  const strokeDashoffset = circumference * (1 - progress);

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
          stroke="#FFC300"
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
        <Text style={styles.number}>{value}</Text>
        <Text style={styles.alert}>Alert</Text>
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