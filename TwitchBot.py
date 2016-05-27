# The base code for connecting to Twitch.tv and sending private messages is based off code provided by Twitch.tv 
# and a tutorial by Manuel Kraus aka Gexo at https://www.youtube.com/watch?v=5Kv3_V5wFgg

# Majority of the custom code can be found where the bot checks for specific messages and provides custom output

import socket, string
import requests
import BeautifulSoup
import re
 
# Set all the variables necessary to connect to Twitch IRC
HOST = "irc.chat.twitch.tv"
NICK = "USERNAME"
PORT = 6667
PASS = "oauth:OAUTH_KEY"
readbuffer = ""
MODT = False
pingNum = 0
 
# Connecting to Twitch IRC by passing credentials and joining a certain channel
s = socket.socket()
s.connect((HOST, PORT))
s.send("PASS " + PASS + "\r\n")
s.send("NICK " + NICK + "\r\n")
s.send("JOIN #USERNAME \r\n")
 
# Method for sending a message
def Send_message(message):
    s.send("PRIVMSG #USERNAME :" + message + "\r\n")
 
 
while True:
    readbuffer = readbuffer + s.recv(1024)
    temp = string.split(readbuffer, "\n")
    readbuffer = temp.pop()
 
    for line in temp:
        # Checks whether the message is PING because its a method of Twitch to check if you're afk
        if (line[0] == "PING"):
            s.send("PONG %s%d\r\n" % line[1] % pingNum)
            pingNum += 1
            print line
        else:
            # Splits the given string so we can work with it better
            parts = string.split(line, ":")
 
            if "QUIT" not in parts[1] and "JOIN" not in parts[1] and "PART" not in parts[1]:
                try:
                    # Sets the message variable to the actual message sent
                    message = parts[2][:len(parts[2]) - 1]
                except:
                    message = ""
                # Sets the username variable to the actual username
                usernamesplit = string.split(parts[1], "!")
                username = usernamesplit[0]
               
                # Only works after twitch is done announcing stuff (MODT = Message of the day)
                # THESE IF CHECKS ARE CUSTOM CODE BY BOSTON MAK
                if MODT:
                    print username + ": " + message
                   
                    # You can add all your plain commands here

                    if message == "!server":
                        session = requests.session()
                        req = session.get('https://www.realmeye.com/player/Bonanah')
                        doc = BeautifulSoup.BeautifulSoup(req.content)
                        Send_message("According to RealmEye, I was last seen" + doc.find(text=re.compile("at .* as .*")))
                        print "According to RealmEye, I was last seen" + doc.find(text=re.compile("at .* .* as .*"))
 
                    if re.match("\?/8\?*", message):
                        session = requests.session()
                        req = session.get('https://www.realmeye.com/player/Bonanah')
                        doc = BeautifulSoup.BeautifulSoup(req.content)
                        stats = doc.find('span', { "class" : "player-stats" })
                        currentClass = stats.parent
                        while re.match("\[u'[A-Za-z]+'", str(currentClass.contents)) == None:
                            currentClass = currentClass.previousSibling
                        currentClass = str(currentClass.contents)[3:len(str(currentClass.contents)) - 2]
                        Send_message("According to Realmeye, my {0} is {1}".format(currentClass, stats.find(text=re.compile("[0-8]/8"))))
                        print "According to Realmeye, my {0} is {1}".format(currentClass, stats.find(text=re.compile("[0-8]/8")))
                
                for l in parts:
                    if "End of /NAMES list" in l:
                        MODT = True