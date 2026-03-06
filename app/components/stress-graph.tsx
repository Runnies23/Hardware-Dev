import React from "react";
import { View, Dimensions } from "react-native";
import { LineChart } from "react-native-chart-kit";

const screenWidth = Dimensions.get("window").width;

export default function StressGraph() {
  const data = {
    labels: ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
    datasets: [
      {
        data: [45, 38, 52, 34, 42, 28, 32],
      },
    ],
  };

  return (
    <View>
      <LineChart
        data={data}
        width={screenWidth * 0.8}
        height={200}
        yAxisInterval={1}
        chartConfig={{
          backgroundGradientFrom: "#ffffff",
          backgroundGradientTo: "#ffffff",
          decimalPlaces: 0,
          color: () => "#17BEBB",
          labelColor: () => "#777",
          propsForDots: {
            r: "5",
            strokeWidth: "2",
            stroke: "#17BEBB",
          },
        }}
        bezier
        style={{
          borderRadius: 16,
        }}
      />
    </View>
  );
}