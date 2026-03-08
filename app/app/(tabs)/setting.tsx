import React, { useState, useEffect } from "react";
import AsyncStorage from "@react-native-async-storage/async-storage";
import {
  View,
  Text,
  StyleSheet,
  TouchableOpacity,
  TextInput
} from "react-native";
import Slider from "@react-native-community/slider";

type SoundOption = {
  id: string;
  icon: string;
  title: string;
  subtitle: string;
};



const sounds: SoundOption[] = [
  { id: "nature", icon: "🌿", title: "เสียงธรรมชาติ", subtitle: "Take some deep breaths" },
  { id: "piano", icon: "🎹", title: "เสียงเปียโน", subtitle: "Take some deep breaths" },
  { id: "therapy", icon: "✨", title: "ดนตรีบำบัด", subtitle: "Take some deep breaths" },
  { id: "meditation", icon: "🧘", title: "ทำสมาธิ", subtitle: "Take some deep breaths" }
];

export default function SettingsScreen() {
  const [selected, setSelected] = useState("nature");
  const [volume, setVolume] = useState(65);
  const [name, setName] = useState("");
  const [age, setAge] = useState("");

  useEffect(() => {
    loadSettings();
  }, []);

  const loadSettings = async () => {
    try {
      const settings = await AsyncStorage.getItem("user_settings");

      if (!settings) return;

      const parsed = JSON.parse(settings);

      if (parsed.selectedSound) setSelected(parsed.selectedSound);
      if (parsed.volume !== undefined) setVolume(parsed.volume);
      if (parsed.name) setName(parsed.name);
      if (parsed.age) setAge(parsed.age);

      console.log("Loaded settings:", parsed);
    } catch (err) {
      console.log("Load settings error:", err);
    }
  };

  const saveSettings = async () => {
    try {
      const data = {
        selectedSound: selected,
        volume: volume,
        name: name,
        age: age
      };

      await AsyncStorage.setItem("user_settings", JSON.stringify(data));

      console.log("Settings saved:", data);
    } catch (e) {
      console.log("Save error", e);
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>การตั้งค่า</Text>

      {sounds.map((sound) => {
        const active = selected === sound.id;

        return (
          <TouchableOpacity
            key={sound.id}
            style={[
              styles.soundCard,
              active && styles.soundCardActive
            ]}
            onPress={() => setSelected(sound.id)}
          >
            <Text style={styles.icon}>{sound.icon}</Text>

            <View style={{ flex: 1 }}>
              <Text style={styles.soundTitle}>{sound.title}</Text>
              <Text style={styles.subtitle}>{sound.subtitle}</Text>
            </View>

            {active && (
              <View style={styles.checkCircle}>
                <Text style={{ color: "white", fontWeight: "bold" }}>✓</Text>
              </View>
            )}
          </TouchableOpacity>
        );
      })}

      {/* Volume */}
      <View style={styles.volumeCard}>
        <Text style={styles.soundTitle}>🔊 ระดับเสียง</Text>

        <Slider
          minimumValue={0}
          maximumValue={100}
          value={volume}
          step={1}
          onValueChange={(value) => setVolume(Number(value.toFixed(1)))}
          minimumTrackTintColor="#17BEBB"
          maximumTrackTintColor="#CFE7EA"
        />

        <Text style={styles.volumeText}>{volume}%</Text>
      </View>

      {/* Name */}
      <View style={styles.inputCard}>
        <TextInput
          placeholder="ชื่อ-สกุล"
          value={name}
          onChangeText={setName}
        />
      </View>

      {/* Age */}
      <View style={styles.inputCard}>
        <TextInput
          placeholder="อายุ"
          value={age}
          onChangeText={setAge}
          keyboardType="numeric"
        />
      </View>

      {/* Save Button */}
      <TouchableOpacity style={styles.button} onPress={saveSettings}>
        <Text style={styles.buttonText}>บันทึก</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({

container: {
  flex: 1,
  backgroundColor: "#E8EFF1",
  padding: 20
},

title: {
  fontSize: 28,
  fontWeight: "bold",
  marginBottom: 20
},

soundCard: {
  flexDirection: "row",
  alignItems: "center",
  backgroundColor: "#F2F2F2",
  borderRadius: 20,
  padding: 16,
  marginBottom: 12,
  shadowColor: "#000",
  shadowOpacity: 0.1,
  shadowRadius: 6,
  elevation: 3
},

soundCardActive: {
  borderWidth: 2,
  borderColor: "#17BEBB"
},

icon: {
  fontSize: 26,
  marginRight: 12
},

soundTitle: {
  fontSize: 18,
  fontWeight: "600"
},

subtitle: {
  color: "#777"
},

checkCircle: {
  width: 36,
  height: 36,
  borderRadius: 18,
  backgroundColor: "#17BEBB",
  alignItems: "center",
  justifyContent: "center"
},

volumeCard: {
  backgroundColor: "#F2F2F2",
  padding: 16,
  borderRadius: 20,
  marginTop: 10
},

volumeText: {
  alignSelf: "flex-end",
  fontSize: 18,
  fontWeight: "bold"
},

inputCard: {
  backgroundColor: "#F2F2F2",
  padding: 16,
  borderRadius: 20,
  marginTop: 14
},

button: {
  backgroundColor: "#17BEBB",
  marginTop: 25,
  padding: 16,
  borderRadius: 25,
  alignItems: "center"
},

buttonText: {
  color: "white",
  fontSize: 18,
  fontWeight: "bold"
}

});