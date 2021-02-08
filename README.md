# Malicious_MPSI

**Running the protocol (we used ubuntu 18.04) :**

1) Clone the repository `git clone https://github.com/ArielCyber/Malicious_MPSI.git`
   <br>Go to the **Malicious_MPSI** directory, by using the command `cd Malicious_MPSI`
   
2) Run the command `sudo bash install.sh`

3) After the installation, a folder named **libOTe** should be created.
   <br>Run the command `cp -r simple/* libOTe/`
 
4) Go to the **libOTe** folder, by using the command `cd libOTe`

5) clone the **google test** library, by running the command `git clone https://github.com/google/googletest.git`

6) Run the command `make`. A file named **bin/main** will be created.

7) Change the ips in the file **ip.txt** to the parties' ips.
   <br> Each party will get a number in range [0..(parties-1)], accordingly to the order in the file.

8) You should change the data in the data.txt, to the data of each party (comma separated numbers).

9) You can change the parameters in the **cfg.txt** file.

10) To activate the protocol-
   <br> Run at every computer the command `./bin/main <party number> cfg.txt`

