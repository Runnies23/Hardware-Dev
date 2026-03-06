// import { useEffect, useRef } from "react";
// import mqtt from "mqtt";
// import Sound from "react-native-sound";

// export default function useMQTTAlert() {
//   const alertSound = useRef(
//     new Sound("alert.mp3", Sound.MAIN_BUNDLE, (error) => {
//       if (error) {
//         console.log("Failed to load sound:", error);
//       }
//     })
//   );

//   const playSound = () => {
//     const sound = alertSound.current;

//     sound.stop(() => {
//       sound.play((success) => {
//         if (!success) {
//           console.log("Sound playback failed");
//         }
//       });
//     });
//   };

//   useEffect(() => {
//     const client = mqtt.connect("ws://iot.cpe.ku.ac.th:9001", {
//       username: "b6810503731",
//       password: "panparin.r@ku.th",
//       reconnectPeriod: 5000,
//     });

//     client.on("connect", () => {
//       console.log("Alert MQTT Connected");

//       client.subscribe("b6810503731/light", (err) => {
//         if (!err) {
//           console.log("Subscribed to alert topic");
//         }
//       });
//     });

//     client.on("message", (topic, message) => {
//       const msg = message.toString();
        
//       console.log("message : " ,msg);
      
//       if (topic === "b6810503731/light" && msg === "1") {
//         console.log("ALERT TRIGGERED");
//         playSound();
//       }
//     });

//     client.on("error", (err) => {
//       console.log("MQTT Error:", err);
//     });

//     return () => {
//       client.end();
//       alertSound.current.release();
//     };
//   }, []);
// }