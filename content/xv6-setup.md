# Instructions for setting up xv6 on CADE servers

Xv6 is a real operating system kernel, and hence, it needs real hardware to boot. Fortunately, today we can emulate hardware in software. Programs like QEMU can emulate functionality of the real physical CPU in software. I.e., QEMU implements the normal CPU loop similar to the one we discussed in class: fetches an instruction pointed by the instruction pointer register (EIP), decodes it, performs all permission and condition checks, computes the outcome, increments EIP and continues to the next instruction. Like a real PC platform, QEMU emulates hardware boot protocol. QEMU starts by loading the disk sector number 0 into the memory location 0x7c00 and jumping to it. Xv6 takes it from there. At a high level, for xv6 it does not matter if it runs on the real hardware or under QEMU. Of course, emulation is slower than real hardware, but besides that as long as QEMU implements the logic of the CPU correctly we do not see any deviations from a real baremetal execution. Surprisingly, QEMU is reasonably fast, so you as a human barely notice the difference.

## SSH into CADE

To configure your xv6 environment, login into one of CADE servers, e.g., using the campus VPN, pick a random 
server by replacing `XX` with a random number:

```
ssh <your-uid>@lab1-XX.eng.utah.edu
```

### Make sure Qemu is in your path (Use bash for all the following commands)

I have build a version of Qemu emulator for you. You only need to add it to your path. You can either add it every time you log in to the circinus machine by exporting the PATH variable, or you can add this line to .bash_profile file in your home folder.
```
export PATH=$PATH:/home/cs5460/qemu-install/bin
```
If you decide to add the PATH permanently to your .bash_profile the line should look something like
```
export PATH=/home/cs5460/qemu-install/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/games:/usr/local/sbin:/usr/local/bin:/usr/X11R6/bin:$HOME/bin:$PATH
```
Add this line to your ~/.bash_profile and ~/.bashrc file and then restart bash(this step is necessary if you are on CADE)


### Clone, build, and boot xv6

If you haven't done this, create `cs5460` folder for all projects in this class. 

```
cd ~/
mkdir cs5460
```

Change into the `cs5460` folder and clone the xv6 repository.

```
cd ~/cs5460
git clone https://github.com/mit-pdos/xv6-public.git
Cloning into xv6...
...
```

Build xv6 (you only will see a bunch of output from the make command that invokes the gcc compiler on all kernel files, links the kernel with ld, creates the kernel file system with all user-level programs that will be available inside xv6, and so on):

```
$ cd xv6-public 
$ make 
```

You're now ready to run xv6.

```
$ make qemu-nox ...  
```

You are now running xv6 on top a hardware platform that is emulated by the Qemu emulator. You are now done with the xv6 setup and can continue moving to any homeworks that are currently assigned. To exit xv6 running under QEMU you can terminate it with **Ctrl-A X**.

You can find more information about QEMU monitor and GDB debugger [here](https://pdos.csail.mit.edu/6.828/2018/labguide.html), feel free to explore them.

## Alternative ways to install xv6 if you feel like it (not required for this class)


### Xv6 on your own Linux system

To run xv6 we need to compile and install a version of the QEMU emulator.  From inside your home folder create a new directory for this class. I suggest you start in your home folder (unless you know what you're doing this is a good way to go)
```
cd ~
mkdir cs5460
```
Change into this new directory
```
cd cs5460
```
Clone the qemu distribution into the qemu folder
```
git clone https://gitlab.com/qemu-project/qemu.git qemu 
```
Change into qemu folder. Configure qemu with minimal settings and the prefix pointing to the install folder
```
./configure --disable-kvm --disable-werror --prefix=/home/<YourUID>/cs5460/qemu-install --target-list="i386-softmmu x86_64-softmmu"
```
If on CADE, you likely need to build an older version just to ensure compatibility with CADE libraries that are a bit behind. Checkout 
and older version: 

```
git checkout remotes/origin/stable-6.0
```

Now configure the build:

```
./configure --disable-kvm --disable-werror --prefix=/home/<YourUID>/cs5460/qemu-install --target-list="i386-softmmu x86_64-softmmu" --python=/usr/bin/python2
```
For example, for me this becomes:
```
./configure --disable-kvm --disable-werror --prefix=/home/u0478645/cs5460/qemu-install --target-list="i386-softmmu x86_64-softmmu"
```
Make and install qemu (this will take some time)
```
make -j 16
make install
```
To make qemu accessible from other programs add it to your path. You can either add it every time you log in to the circinus machine by exporting the PATH variable, or you can add this line to .bash_profile file in your home folder.
```
export PATH=$PATH:$HOME/cs5460/qemu-install/bin
```
If you decide to add the PATH permanently to your .bash_profile the line should look something like
```
export PATH=$HOME/cs5460/qemu-install/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/games:/usr/local/sbin:/usr/local/bin:/usr/X11R6/bin:$HOME/bin:$PATH
```

### Xv6 on Mac 

On the new ARM-based CPUs you need to cross-compile everything to get the x86 qemu running. The easiest way to do this is to use the Nix environment we provide:

```
git clone https://github.com/mars-research/xv6-cs5460.git
```

Install Nix (follow the Nix installation instructions online: (https://nixcademy.com/posts/nix-on-macos/)[https://nixcademy.com/posts/nix-on-macos/], then enter

```
nix develop
```

You can now build xv6 inside the Nix shell.

### XV6 in Docker

In case you want to use run xv6 on your own machine using docker containers, you can try it out as well. I have successfully built XV6 using the grantbot/xv6 image hosted in the [docker hub](https://hub.docker.com/r/grantbot/xv6/)


In my localmachine, I downloaded the XV6 source code as follows :
```
localhost$ mkdir XV6_Dev
localhost$ cd XV6_Dev
localhost$ git clone git://github.com/mit-pdos/xv6-public.git
Cloning into xv6...
...
```
Next, you will need to setup Docker, if you don't have it already on your machine. I followed the instructions from here. You will find similar instructions for other OS as well in the docker website. Once you have the setup ready, download the ***grantbot/xv6*** image using
```
docker pull grantbot/xv6
```
Then you can start the container using
```
docker run -v '/{Path to local XV6 folder}/XV6_Dev':/home/a/XV6_Dev/ -i -t grantbot/xv6
```
Once you have the bash prompt you can type the following to start XV6,
```
cd ~/XV6_Dev/
make qemu-nox
```
