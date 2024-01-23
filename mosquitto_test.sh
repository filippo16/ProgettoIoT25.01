#!/bin/bash

# Verifica che sia stato fornito almeno un parametro
if [ $# -eq 0 ]; then
  echo "Usage: $0 <parametro>"
  exit 1
fi

# Assegna il primo parametro fornito a una variabile
mio_parametro=$1

# Utilizza il parametro nella logica dello script
echo "Hai fornito il parametro: $mio_parametro"
mosquitto_pub -h 192.168.70.57 -t code -m "$1"
