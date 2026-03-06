import React from "react";
import { View, Text, StyleSheet } from "react-native";

type Props = {
  icon: React.ReactNode
  title: string
  subtitle: string
  value: number
  iconBackground: string
}

export default function SummaryCard({
  icon,
  title,
  subtitle,
  value,
  iconBackground
}: Props) {
  return (
    <View style={styles.card}>
      <View style={[styles.iconContainer, { backgroundColor: iconBackground }]}>
        {icon}
      </View>

      <Text style={styles.title}>{title}</Text>
      <Text style={styles.subtitle}>{subtitle}</Text>
      <Text style={styles.value}>{value}</Text>
    </View>
  )
}

const styles = StyleSheet.create({
  card: {
    backgroundColor: "#F2F2F2",
    borderRadius: 20,
    padding: 18,
    alignItems: "center",
    width: 150,
    shadowColor: "#000",
    shadowOpacity: 0.1,
    shadowRadius: 6,
    elevation: 4,
    marginHorizontal: 8
  },

  iconContainer: {
    width: 40,
    height: 40,
    borderRadius: 10,
    justifyContent: "center",
    alignItems: "center",
    marginBottom: 8
  },

  title: {
    fontSize: 16,
    fontWeight: "600"
  },

  subtitle: {
    fontSize: 12,
    color: "#777"
  },

  value: {
    fontSize: 28,
    fontWeight: "bold",
    marginTop: 4
  }
});