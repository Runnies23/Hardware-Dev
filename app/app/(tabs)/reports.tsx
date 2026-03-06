import React from "react";
import { View, Text, StyleSheet, ScrollView } from "react-native"; // 👈 add ScrollView
import { Ionicons } from "@expo/vector-icons";
import StressGraph from "@/components/stress-graph";
import HeartRateGraph from "@/components/heart-rate-graph";
import SummaryCard from "@/components/summarycard";

const SUMMARY_CARDS = [
  {
    id: "stress",
    icon: <Ionicons name="analytics" size={20} color="#14B8A6" />,
    title: "ความเครียด",
    subtitle: "เฉลี่ย",
    value: 39,
    iconBackground: "#DDF3F2",
  },
  {
    id: "heart-rate",
    icon: <Ionicons name="heart-outline" size={20} color="#F87171" />,
    title: "อัตราเต้นหัวใจ",
    subtitle: "เฉลี่ย",
    value: 70,
    iconBackground: "#FDE2E4",
  },
] as const;

const GRAPH_CARDS = [
  { id: "stress", title: "ความเครียด", Graph: StressGraph },
  { id: "heart-rate", title: "อัตราเต้นหัวใจ", Graph: HeartRateGraph },
] as const;

const SummaryRow = () => (
  <View style={styles.row}>
    {SUMMARY_CARDS.map((card) => (
      <SummaryCard key={card.id} {...card} />
    ))}
  </View>
);

const GraphCard = ({ title, Graph }: { title: string; Graph: React.ComponentType }) => (
  <View style={styles.card}>
    <Text style={styles.cardTitle}>{title}</Text>
    <Graph />
  </View>
);

export default function ReportsScreen() {
  return (
    <ScrollView
      style={styles.screen}
      contentContainerStyle={styles.content}
      showsVerticalScrollIndicator={false}
    >
      {/* Header */}
      <View style={styles.header}>
        <Text style={styles.title}>ผลสรุป</Text>
      </View>

      {/* Summary cards row */}
      <SummaryRow />

      {/* Graph section */}
      <View style={styles.graphSection}>
        {GRAPH_CARDS.map(({ id, title, Graph }) => (
          <GraphCard key={id} title={title} Graph={Graph} />
        ))}
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
    backgroundColor: "#E8EFF1",
  },

  content: {
    paddingBottom: 40,
  },

  header: {
    paddingHorizontal: 20,
    paddingTop: 20,
    paddingBottom: 10,
  },

  title: {
    fontSize: 26,
    fontWeight: "bold",
  },

  graphSection: {
    paddingHorizontal: 20,
    paddingTop: 10,
  },

  card: {
    backgroundColor: "#FFF",
    padding: 16,
    borderRadius: 20,
    marginBottom: 20,
  },

  cardTitle: {
    fontSize: 18,
    fontWeight: "600",
    marginBottom: 10,
  },

  row: {
    flexDirection: "row",
    justifyContent: "center",
    alignItems: "center",
    gap: 16, // 👈 spacing between cards (React Native 0.71+)
    paddingVertical: 20,
  },
});