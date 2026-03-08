import { Image } from 'expo-image';
import React, { useEffect, useRef } from "react";
import { Platform, StyleSheet, View, Text } from 'react-native';
// import mqtt from "mqtt";
import StressCircle from '@/components/StressCircle';
import InfoCard from '@/components/InfoCard';
import useMQTTBpm from '@/components/mqtt/bpm';
import useMQTTStress from '@/components/mqtt/stress';
import useMQTTStress_level from '@/components/mqtt/level';
import useMQTTAlert from '@/components/mqtt/sound';

export default function HomeScreen() {

  type StressConfig = {
    colour: string;
    iconBackground: string;
    status: string;
  };

  const HRV = useMQTTStress();
  const StressLevel = useMQTTStress_level();
  const Bpm = useMQTTBpm();

  const stressConfig: Record<number, StressConfig> = {
    0: { colour: "#22C55E", iconBackground: "#DCFCE7", status: "ผ่อนคลาย" },
    1: { colour: "#14B8A6", iconBackground: "#DDF3F2", status: "ปกติ" },
    2: { colour: "#F59E0B", iconBackground: "#FEF3C7", status: "มีความเครียด" },
    3: { colour: "#EF4444", iconBackground: "#FEE2E2", status: "ความเครียดสูง" },
  };
  const stressconfig = stressConfig[StressLevel] || stressConfig[0];

  type StatusConfig = {
  colour: string;
  iconBackground: string;
  status: string;
  };

  const BPMConfig: Record<number, StatusConfig> = {
    0: { colour: "#3B82F6", iconBackground: "#DBEAFE", status: "ชีพจรต่ำ" },
    1: { colour: "#22C55E", iconBackground: "#DCFCE7", status: "ปกติ" },
    2: { colour: "#F59E0B", iconBackground: "#FEF3C7", status: "ชีพจรสูง" },
    3: { colour: "#EF4444", iconBackground: "#FEE2E2", status: "ชีพจรสูงมาก" },
  };
  const getBpmLevel = (bpm: number) => {
    if (bpm < 60) return 0;
    if (bpm <= 100) return 1;
    if (bpm <= 120) return 2;
    return 3;
  };


  const bpmLevel = getBpmLevel(Bpm);
  const bpmconfig = BPMConfig[bpmLevel];;
  // const clientRef = useRef(null);
  // const sound_mp3_list = []
  useMQTTAlert();

  return (
    <View style={styles.container}>
      <Text style={styles.title}>ค่าความเครียด</Text>
      <StressCircle value={HRV} />

      <View style={styles.cards}>
        <InfoCard 
            icon="heart" 
            label="อัตราเต้นหัวใจ" 
            value={Bpm}
            unit="BPM"
            colour={bpmconfig.colour}
            iconBackground={bpmconfig.iconBackground}

        />

        <InfoCard 
            icon="pulse" 
            label="สถานะ" 
            value={stressconfig.status}
            colour={stressconfig.colour}
            iconBackground={stressconfig.iconBackground}
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#EDEFF0",
    paddingTop: 60,
    alignItems: "center",
  },

  iconContainer: {
    width: 70,
    height: 70,
    borderRadius: 20,
    backgroundColor: "#FFE5E5",
    justifyContent: "center",
    alignItems: "center",
    marginBottom: 10,
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