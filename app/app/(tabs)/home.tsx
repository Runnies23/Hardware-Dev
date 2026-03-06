import { Image } from 'expo-image';
import React, { useEffect, useRef } from "react";
import { Platform, StyleSheet, View, Text } from 'react-native';
// import mqtt from "mqtt";
import StressCircle from '@/components/StressCircle';
import InfoCard from '@/components/InfoCard';
import useMQTTStress from '@/components/mqtt/stress';
// import useMQTTAlert from '@/components/mqtt/sound';

export default function HomeScreen() {

  const stress = useMQTTStress();
  // const clientRef = useRef(null);
  // const sound_mp3_list = []
  // useMQTTAlert();


  return (
    <View style={styles.container}>
      <Text style={styles.title}>ค่าความเครียด</Text>
      <StressCircle value={stress} />

      <View style={styles.cards}>
        <InfoCard 
            icon="heart" 
            label="อัตราเต้นหัวใจ" 
            value="77" 
            unit="BPM" 
            colour="#FF6B6B" 
            iconBackground="#FDE2E4"
        />

        <InfoCard 
            icon="pulse" 
            label="สถานะ" 
            value="ปกติ" 
            colour="#14B8A6" 
            iconBackground="#DDF3F2"
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