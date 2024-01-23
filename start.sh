#!/bin/bash

/opt/lampp/lampp startmysql
/opt/lampp/lampp startapache
/opt/lampp/lampp status
systemctl status mosquitto

echo "------------------------------------"

cd /home/filippo/Scrivania/Project/project
npm run dev
