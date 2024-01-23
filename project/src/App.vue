<script lang="ts">
  import { defineComponent } from 'vue';
  import { Dipendente, Ordine } from './types';
  import mqtt from 'mqtt';

  export default defineComponent({
    data() {
      return {
        jsonMessage: null as Dipendente | null,
        dataOrdine: [] as Ordine[] | null
      }
    },
    methods: {
      getData() {
        const client = mqtt.connect('ws://192.168.70.57:9001');
  
        client.on('connect', () => {
          console.log("Connesso al broker!");
          client.subscribe('dataToClient');
          client.subscribe('dataOrder');
          client.subscribe('error');
        });

        client.on('message', (topic: String, message: Buffer) => {
          if(topic == 'dataToClient') {
            console.log("Messaggio ricevuto nel topic ", topic);
            this.jsonMessage = JSON.parse(message.toString()); // arrivo della stringa json del dipendente
            console.log("JSON MESSAGE---- DEBUG: ", this.jsonMessage);
          } else if(topic == 'dataOrder') {
            console.log("Messaggio ricevuto nel topic ", topic);
            this.dataOrdine = JSON.parse(message.toString());
            console.log("DATA ORDINE --- DEBUG: ", this.dataOrdine); // arrivo stringa json degli ordini
          } else if(topic == 'error') {
            this.jsonMessage = null; // annullo le casello in caso di errore
            this.dataOrdine = null;
            alert(message.toString());
          }
          
          
        });
      }
    },
    mounted() {
      this.getData();
    }
  });
</script>

<template>
  <div class="container">
    <h1>Dati JSON Ricevuti</h1>
    <p v-if="jsonMessage"><strong>Nome:</strong> {{ jsonMessage?.nome }}</p>
    <p v-if="jsonMessage"><strong>Cognome:</strong> {{ jsonMessage?.cognome }}</p>
    <p v-if="jsonMessage"><strong>Ruolo:</strong> {{ jsonMessage?.ruolo }}</p>

    <div v-if="dataOrdine && dataOrdine.length > 0" class="order-section">
      <h2>Ordini</h2>
      <ul>
        <li v-for="(ordine, index) in dataOrdine" :key="index" class="order-item">
          <strong>ID Ordine:</strong> {{ ordine.idordine }} - <strong>Prodotto:</strong> {{ ordine.contiene }} - <strong>Quantità:</strong> {{ ordine.quantita }}
        </li>
      </ul>
    </div>

    <p v-if="!jsonMessage" class="no-data-message">
      Nessun dato JSON disponibile al momento.
    </p>
  </div>
</template>

<style scoped>
  .container {
    padding: 20px;
    margin: 20px;
    border: 1px solid #ccc;
    border-radius: 8px;
  }

  h1 {
    color: green;
    margin-bottom: 20px;
  }

  p {
    margin-bottom: 10px;
  }

  p strong {
    font-weight: bold;
  }

  .order-section {
    margin-top: 20px;
  }

  ul {
    list-style: none;
    padding: 0;
  }

  .order-item {
    margin-bottom: 10px;
    border-bottom: 1px solid #ddd;
    padding-bottom: 5px;
  }

  .no-data-message {
    font-style: italic;
  }
</style>
