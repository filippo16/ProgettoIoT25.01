//import mqtt from 'mqtt';
import { Client, Message } from 'paho-mqtt';

export function getDataMQTT() {
    const client = new Client('192.168.70.57', 9001, 'client-mqtt-1');
    
    const onConnect = () => {
      console.log("Connesso al broker!");
      client.subscribe('dataToClient');
      client.subscribe('error');
    }

    function onMessageArrived(message: Message) {
      console.log(`Messaggio ricevuto su ${message.destinationName}: ${message.payloadString}`);
      try {
        const jsonData = JSON.parse(message.payloadString);
        console.log('Dati JSON ricevuti:', jsonData);
        return jsonData;
      } catch (error) {
        console.error('Errore durante il parsing del messaggio JSON:', error);
      }
    }
    client.onMessageArrived = onMessageArrived;

    client.connect({ onSuccess: onConnect });
  }
/*
  getDataMQTT() {
    let client: any;

    if(!client){
      client = mqtt.connect('mqtt://192.168.1.29');
    }
  
    client.on('connect', () => {
      console.log("Connesso al broker!");
      client.subscribe('dataToClient');
      client.subscribe('error');
    });

    client.on('message', (topic: String, message: Buffer) => {
      console.log("Messaggio ricevuto nel topic ", topic);
      this.jsonMessage = JSON.parse(message.toString());
    });*/

    /* 
    const client = new Client('192.168.70.57', 9001, 'client-mqtt-1');

        const onConnect = () => {
          console.log("Connesso al broker!");
          client.subscribe('dataToClient');
          client.subscribe('error');
        }

        function onMessageArrived(message: Message) {
          console.log(`Messaggio ricevuto su ${message.destinationName}: ${message.payloadString}`);
          try {
           const jsonMessage = JSON.parse(message.payloadString);
          } catch (error) {
            console.error('Errore durante il parsing del messaggio JSON:', error);
          }
        }
        client.onMessageArrived = onMessageArrived;
        
        client.connect({ onSuccess: onConnect });
    */