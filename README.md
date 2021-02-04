# Malicious_MPSI


1) Download the repository

2) Run the command 'sudo bash install.sh'

3) After the installation, you should see that folder named **libOTe** has been created.
   <br>Run the command cp -r simple/* libOTe/
 
4) Go to the **libOTe** folder, by using the command **cd libOTe**

5) Run the command **make**. A file named **bin/main** will be created.

6) Change the ips in the file **ip.txt** to your computers' ip.

7) You should change the data in the data.txt, to the data of each party (comma separated numbers).

8) You can change the parameters at the **cfg.txt** file.

9) Run at every computer the command **./bin/main <<a>party number> cfg.txt**.
   <br> The number of each party is different and need to be chosen from {0..(parties-1)}

