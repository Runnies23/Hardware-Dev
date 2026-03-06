import React from "react";
import { View, Dimensions } from "react-native";
import { BarChart } from "react-native-chart-kit";

const screenWidth = Dimensions.get("window").width;

export default function HeartRateGraph() {
  const data = {
    labels: ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
    datasets: [
      {
        data: [72, 68, 75, 70, 73, 65, 69],
      },
    ],
  };

  return (
    <View>
      <BarChart
        data={data}
        width={screenWidth * 0.8}
        height={200}
        yAxisLabel=""
        yAxisSuffix=""
        chartConfig={{
          backgroundGradientFrom: "#ffffff",
          backgroundGradientTo: "#ffffff",
          decimalPlaces: 0,
          color: () => "#F26C7C",
          labelColor: () => "#777",
        }}
        style={{
          borderRadius: 16,
        }}
      />
    </View>
  );
}