# finetd

Finetd is a xinetd inspired super-server that loads services on demand based on socket events.

# Why?
Finetd created to help to run services on low power devices such as raspberrypi. I tried to run a git server (gitea) on *Raspeberrypi Zero W*! 
Gitea was running smoothly, however it was consuming quite a chunk of cpu load even when it was idle. Finetd solved this issue 
by loading required server on demand, finetd listened on proxy port and forwarded requests to port where gitea was running. 
finetd stops server if there is no request to the server for specified time.

# Usage
finetd --config config_path --loglevel loglevelNumber -t timeOutSeconds

# Sample config file 

*format: SourcePort DestPort start command ; optional stop command*    

    3000 5000 serve -l 5000;kill -INT %p
    8080 8000 python -m http.server 8000
    4444 5555 pm2 start servicea; pm2 stop servicea

*Requests listened at port 3000 and forwarded to service running at port 5000, on timeout service killed using signal INT(stop command specified after semicolon).
In second line port 8080 forwarded to 8000 and stop command is absent which is defaulted to kill command.*  
