import paho.mqtt.client as mqtt
import mysql.connector as mysql
import json

config = {  #configurazione per il db
    'user': 'root',
    'host': 'localhost',
    'password': '',
    'database': 'dbprova'
}

ultimo_code = [] #"lista" di ultimi codici, avrei potuto usare un DB dedicato alle ultime letture però per semplificare ho utlizzato un array (magari in quel DB avrei potuto mettere l'id carta letto e l'id del client da chi arrivava l'id letto)

dataToClient = {
    'id': "",
    'nome': "",
    'cognome': "",
    'ruolo': ""
}

def build_data(id, nome, cognome, ruolo):
    dataToClient['id'] = id
    dataToClient['nome'] = nome
    dataToClient['cognome'] = cognome
    dataToClient['ruolo'] = ruolo

def sendCodeData(cursor, code): # funione che serve per prendere i dati del dipendente e dei vari ordini e che vengono mandati al webserver.
    cursor.execute("SELECT id, nome, cognome, ruolo FROM utenti WHERE id = %s LIMIT 1", (code,))
    catchInfo = cursor.fetchone()

    #ultimo_code.append(code) avrei dovuto metterlo qua se addPerson leggesse l'ultima carta senza immetere l'id manualmente
    if(catchInfo): # esiste il dipendente cercato
        if(catchInfo[0] == code): # codice uguale
            ultimo_code.append(code)
            cursor.execute("SELECT idordine, contiene, quantita FROM ordine WHERE ordinante = %s", (catchInfo[0],))
            catchOrder = cursor.fetchall()
            print("-------------------------INVIO DATACLIENT-----------------------------")
            build_data(catchInfo[0], catchInfo[1], catchInfo[2], catchInfo[3])
            client.publish("dataToClient", json.dumps(dataToClient))

            print("-------------------------INVIO DATAORDER-----------------------------")
            dataOrder = [
                {"id": item[0], "contiene": item[1], "quantita": item[2]} for item in catchOrder
            ]
            client.publish("dataOrder", json.dumps(dataOrder))
    else:
        cursor.close()
        print("E001 - Persona non presente nel sistema!")
        client.publish("error", "E001 - Persona non presente nel sistema!") #errore in caso di codice sbagliato

def deletePerson(cursor, ucode): #funzione che prende che elimina il dipendente letto per ultimo
    if(ucode):
        try:
            cursor.execute("DELETE FROM utenti WHERE id = %s", (ucode,))
            print(f"ID: {ucode} eliminato con successo!")
        except mysql.connector.Error as err:
            print("Errore: ", err)
        cursor.close()
    

def addPerson(cursor, msg): #aggiunge un dipendente (a differenza di delete qua l'id lo immetto manualmente, scelta fatta per motivi di diversità tra le due funzioni, avrei potuto mettere un input in più ucode come in deletePerson e l'avrei messo nella query INSERT)
        data_json = json.loads(msg)
        cursor.execute("INSERT INTO utenti (id, nome, cognome, ruolo) VALUES (%s, %s, %s, %s)", (data_json['id'], data_json['nome'], data_json['cognome'], data_json['ruolo']))
        print(f"ID: {data_json['id']} aggiunto con successo!")
        cursor.close()


def on_connect(client, userdata, flags, rc):
    print(f"Start control-broker ||| {rc}")
    client.subscribe("code")
    client.subscribe("delete") # avrei potuto usare il topic "command" e magari come messaggio una stringa json con all'interno un campo type per il tipo di comando e poi 'msg' che avrebbe potuto contenere il necessario per il comando.
    client.subscribe("add")
    

def on_message(client, userdata, msg):
    code = msg.payload.decode()
    print(f"CODE: {code}")
    cursor = conn.cursor()
    if(msg.topic == "code"):
        sendCodeData(cursor, code)
    if(msg.topic == "delete"):
        if(ultimo_code):
            deletePerson(cursor, ultimo_code[-1])
        else: 
            cursor.close()
            print("Non è stata letta alcuna carta")
    if(msg.topic == "add"):
        addPerson(cursor, code)



conn = mysql.connect(**config)
conn.autocommit = True

client = mqtt.Client(userdata={id: "broker-central"})
client.connect("192.168.70.57", 1883, 60)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()