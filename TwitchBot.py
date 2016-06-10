# The base code for connecting to Twitch.tv and sending private messages is based off code provided by Twitch.tv 
# and a tutorial by Manuel Kraus aka Gexo at https://www.youtube.com/watch?v=5Kv3_V5wFgg

# Majority of the custom code can be found where the bot checks for specific messages and provides custom output

import socket, string
import requests
import BeautifulSoup
import re
import json
import time
 
# Set all the variables necessary to connect to Twitch IRC
HOST = "irc.chat.twitch.tv"
NICK = "bonanarama"
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
s.send("JOIN #bonanarama \r\n")
 
# Method for sending a message
def send_message(message):
    s.send("PRIVMSG #bonanarama :(CustomBot) " + message + "\r\n")

def get_stats(gameName):
    if gameName.lower() == 'League of Legends'.lower():
        return 'Silver 1... i suck'        
    if gameName.lower() == 'Realm of the Mad God'.lower() or gameName.lower() == 'rotmg':
        session = requests.session()
        req = session.get('https://www.realmeye.com/player/Bonanah')
        doc = BeautifulSoup.BeautifulSoup(req.content)
        stats = doc.find('span', { "class" : "player-stats" })
        return format_realm_stats(stats)        
    if gameName.lower() == 'Overwatch'.lower():
        return 'The cute girl I am playing is 10/10'

    return 'Could not find a game by the name of ' + gameName   

def get_pet_stats(stats):

    stats = stats.contents[1].contents[0]
    petName = str(stats.contents[1])
    petName = petName[4 : len(petName) - 5]
    rarity = str(stats.contents[2])
    rarity = rarity[4 : len(rarity) - 5]

    skillOne = str(stats.contents[5].contents)
    skillOne = skillOne[7 : len(skillOne) - 8]

    skillTwo = str(stats.contents[7].contents)
    skillTwo = skillTwo[7 : len(skillTwo) - 8]

    skillThree = str(stats.contents[9].contents)
    skillThree = skillThree[7 : len(skillThree) - 8]

    skillOneLevel = "0"
    skillTwoLevel = "0"
    skillThreeLevel = "0"

    disabledText = re.compile("class=\"pet-ability-disabled\"\>")
    if disabledText.match(skillOne):
        match = disabledText.match(skillOne)
        skillOne = skillOne[match.end():]
        skillTwo = skillTwo[match.end():]
        skillThree = skillThree[match.end():]
    elif disabledText.match(skillTwo):
        match = disabledText.match(skillTwo)
        skillTwo = skillTwo[match.end():]
        skillThree = skillThree[match.end():]

        skillOneLevel = str(stats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
    elif disabledText.match(skillThree):
        match = disabledText.match(skillThree)
        skillThree = skillThree[match.end():]
        
        skillOneLevel = str(stats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
        skillTwoLevel = str(stats.contents[8].contents[0].contents)
        skillTwoLevel = skillTwoLevel[3 : len(skillTwoLevel) - 2]
    else:
        skillOneLevel = str(stats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
        skillTwoLevel = str(stats.contents[8].contents[0].contents)
        skillTwoLevel = skillTwoLevel[3 : len(skillTwoLevel) - 2]
        skillThreeLevel = str(stats.contents[10].contents[0])
        skillThreeLevel = skillThreeLevel[3 : len(skillThreeLevel) - 2]

    return "My {0} {1} has {2} {3}, {4} {5}, and {6} {7}".format(rarity, petName, skillOneLevel, skillOne, \
        skillTwoLevel, skillTwo, skillThreeLevel, skillThree)

def format_realm_stats(stats):
    baseStats = re.search("data-stats=\"\[[0-9,]*\]\"", str(stats))
    statBonuses = re.search("data-bonuses=\"\[[0-9,]*\]\"", str(stats))
    level = re.search("data-level=\"[0-9]*\"", str(stats))
    if (stats == None or baseStats == None or statBonuses == None):
        return 'Cannot access RealmEye stats at this time FeelsBadMan'
    #convert strings into arrays
    baseStats = str(baseStats.group())[13 : len(str(baseStats.group())) - 2]
    baseStats = baseStats.split(',')
    statBonuses = str(statBonuses.group())[15 : len(str(statBonuses.group())) - 2]
    statBonuses = statBonuses.split(',')
    level = str(level.group())[12 : len(str(level.group())) - 1]
    maxedStats = str(stats.contents)[3 : len(str(stats.contents)) - 2]
    currentClass = stats.parent
    totalStats = ["" for i in range(len(baseStats))]
    for index in range(0, len(baseStats)):
        if statBonuses[index] != '0':
            baseStat = int(baseStats[index]) - int(statBonuses[index])
            totalStats[index] = "{0}(+{1})".format(baseStat, statBonuses[index])
        else:
            totalStats[index] = baseStats[index]
    while re.match("\[u'[A-Za-z]+'", str(currentClass.contents)) == None:
        currentClass = currentClass.previousSibling
    currentClass = str(currentClass.contents)[3 : len(str(currentClass.contents)) - 2]
    return "According to RealmEye, my {0} {1} has {2}HP, {3}MP, {4}ATK, {5}DEF, {6}SPD, {7}VIT, {8}WIS, {9}DEX".format(\
        maxedStats, currentClass, totalStats[0], totalStats[1], totalStats[2], totalStats[3], totalStats[4], \
        totalStats[5], totalStats[6], totalStats[7])

cooldowns = [int(0) for i in range(4)];
 
while True:
    readbuffer = readbuffer + s.recv(1024)
    temp = string.split(readbuffer, "\n")
    readbuffer = temp.pop()
 
    for line in temp:
        # Checks whether the message is PING because its a method of Twitch to check if you're afk
        if ("PING :tmi.twitch.tv" in line):
            s.send("PONG {0}{1}\r\n".format(line[1],pingNum))
            pingNum += 1
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
                    currentTime = int(time.time())

                    if cooldowns[0] + 5 <= currentTime and message.lower() == "!server":
                        req = requests.get('https://www.realmeye.com/player/Bonanah')
                        doc = BeautifulSoup.BeautifulSoup(req.content)
                        server = str(doc.find(text=re.compile("at .* as .*")))
                        send_message("According to RealmEye, I was last seen" + server)
                        print "According to RealmEye, I was last seen" + server
                        cooldowns[0] = currentTime
 
                    if cooldowns[1] + 5 <= currentTime and re.match("!?\?/8*", message):
                        req = requests.get('https://www.realmeye.com/player/Bonanah')
                        doc = BeautifulSoup.BeautifulSoup(req.content)
                        stats = doc.find('span', { "class" : "player-stats" })
                        currentClass = stats.parent
                        while re.match("\[u'[A-Za-z]+'", str(currentClass.contents)) == None:
                            currentClass = currentClass.previousSibling
                        currentClass = str(currentClass.contents)[3 : len(str(currentClass.contents)) - 2]
                        stats = stats.find(text=re.compile("[0-8]/8"))
                        send_message("According to Realmeye, my {0} is {1}".format(currentClass, stats))
                        print "According to Realmeye, my {0} is {1}".format(currentClass, stats)
                        cooldowns[1] = currentTime
                
                    if cooldowns[2] + 5 <= currentTime and message.lower() == "!stats":
                        req = requests.get("https://api.twitch.tv/kraken/streams/bonanarama", headers={'Accept': 'application/vnd.twitchtv.v3+json'})
                        twitchJson = json.loads(req.content)
                        stats = get_stats(twitchJson['stream']['game'])
                        send_message(stats)
                        print stats
                        cooldowns[2] = currentTime
                    elif cooldowns[0] + 5 <= currentTime and re.match("!stats *", message):
                        stats = get_stats(message[7:])
                        send_message(stats)
                        print stats
                        cooldowns[2] = currentTime

                    if cooldowns[3] + 5 <= currentTime and message.lower() == "!pet":
                        req = requests.get('https://www.realmeye.com/pets-of/Bonanah')
                        doc = BeautifulSoup.BeautifulSoup(req.content)
                        stats = doc.find('table', { "class" : "table table-striped tablesorter" })
                        petStats = get_pet_stats(stats)
                        send_message(petStats)
                        print petStats
                        cooldowns[3] = currentTime

                for l in parts:
                    if "End of /NAMES list" in l:
                        MODT = True