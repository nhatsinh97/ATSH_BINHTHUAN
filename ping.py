import os
hostname = "http://192.168.32.205" #example
response = os.system("ping -c 1 " + hostname)
 
#and then check the response...
if response == 0:
    print (hostname, 'is up!')
else:
    print (hostname, 'is down!')