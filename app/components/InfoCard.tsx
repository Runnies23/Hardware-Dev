import React from "react";
import { View, Text, StyleSheet } from "react-native";
import { Ionicons } from "@expo/vector-icons";

type InfoCardProps = {
  icon?: React.ComponentProps<typeof Ionicons>["name"];
  label: string;
  value: string | number;
  unit?: string;
  colour?: string;
  iconBackground?: string;
};

const InfoCard: React.FC<InfoCardProps> = ({
  icon,
  label,
  value,
  unit,
  colour = "#000",
  iconBackground = "#E6E6E6",
}) => {
  return (
    <View style={styles.card}>
      {icon && (
        <View style={[styles.iconBox, { backgroundColor: iconBackground }]}>
          <Ionicons name={icon} size={26} color={colour} />
        </View>
      )}

      <View style={styles.textContainer}>
        <Text style={styles.label}>{label}</Text>

        <Text style={styles.value}>
          {value}
          {unit && <Text style={styles.unit}> {unit}</Text>}
        </Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  card: {
    flexDirection: "row",
    backgroundColor: "#F3F3F3",
    padding: 16,
    borderRadius: 18,
    marginVertical: 10,
    alignItems: "center",
  },

  iconBox: {
    width: 50,
    height: 50,
    borderRadius: 12,
    justifyContent: "center",
    alignItems: "center",
    marginRight: 16,
  },

  textContainer: {
    flex: 1,
  },

  label: {
    color: "#666",
    fontSize: 14,
  },

  value: {
    fontSize: 28,
    fontWeight: "bold",
  },

  unit: {
    fontSize: 14,
    color: "#888",
  },
});

export default InfoCard;