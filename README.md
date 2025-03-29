Pour acceder au serveur Tchat_Live_TCP il vous faut docker.

Une fois docker téléchargé entrez la commande suivante : 
docker run -it -e USER_NAME=(votre user name) -e PORT=(votre port) niko833/tchat_tcp_client

Le pseudo par défaut si il n'est pas spécifé quand vous lancez le container docker sera "invité".
Le port par défaut si il n'est pas spécifé quand vous lancez le container docker sera 4000.
