# Malicious_MPSI

**Running the protocol :**

1) Clone the repository `git clone https://github.com/ArielCyber/Malicious_MPSI.git`

2) Run the command `sudo bash install.sh`

3) After the installation, a folder named **libOTe** should have been created.
   <br>Run the command `cp -r simple/* libOTe/`
 
4) Go to the **libOTe** folder, by using the command `cd libOTe`

5) Run the command `make`. A file named **bin/main** will be created.

6) Change the ips in the file **ip.txt** to your computers' ips.

7) You should change the data in the data.txt, to the data of each party (comma separated numbers).

8) You can change the parameters in the **cfg.txt** file.

9) To activate the protocol-
   <br> Run at every computer the command `./bin/main <party number> cfg.txt`.
   <br> The number of each party has to be different and from this range {0..(parties-1)}

