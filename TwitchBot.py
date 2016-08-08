# The base code for connecting to Twitch.tv and sending private messages is based off code provided by Twitch.tv 
# and a tutorial by Manuel Kraus aka Gexo at https://www.youtube.com/watch?v=5Kv3_V5wFgg

# Majority of the custom code can be found where the bot checks for specific messages and provides custom output

import socket, string
import requests
import BeautifulSoup
import re
import json
import time
from array import array
 
# Set all the variables necessary to connect to Twitch IRC
HOST = "irc.chat.twitch.tv"
NICK = "bonanarama"
PORT = 6667
PASS = "oauth:py38r9f6w72r1cmkv9ugbvuxbpajnw"
RIOTAPI = "RGAPI-35C5ECF7-724A-446E-9255-B76D32726C5F"
SUMMONERID = "34500916"
readbuffer = ""
MODT = False
pingNum = 0


cooldowns = [int(0) for i in range(4)];
updateCooldown = 0
cachedOverwatchLevel = 0
cachedOverwatchRank = 0
cachedRealmStats = 0
cachedPetStats = 0
 
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
        return get_league_of_legends_stats() 
    if gameName.lower() == 'Realm of the Mad God'.lower() or gameName.lower() == 'rotmg':
        return format_realm_stats()        
    if gameName.lower() == 'Overwatch'.lower():
        global cachedOverwatchLevel
        global cachedOverwatchRank
        set_overwatch_stats()
        if int(cachedOverwatchRank) < 40:
            return "I am currently level " + str(cachedOverwatchLevel) + " with a competitve rank of " + str(cachedOverwatchRank) + ". Teach me how to be good please"
        else:        
            return "I am currently level " + str(cachedOverwatchLevel) + " with a competitve rank of " + str(cachedOverwatchRank)

    return 'Could not find a game by the name of ' + gameName + " or could not find any stats for it at this time."   

def get_league_of_legends_stats():
    req = requests.get("https://na.api.pvp.net/api/lol/na/v2.5/league/by-summoner/" + SUMMONERID + "/entry?api_key=" + RIOTAPI)
    soloData = req.json()[SUMMONERID][0]
    tier = soloData["tier"]
    division = soloData["entries"][0]["division"]
    points = soloData["entries"][0]["leaguePoints"]
    if tier.lower() == "silver" or tier.lower() == "bronze":
        return "I am " + tier.title() + " " + division + " with " + str(points) + "LP. I'm pretty bad at this game."
    else:
        return "I am " + tier.title() + " " + division + " with " + str(points) + "LP."

def set_overwatch_stats():
    global cachedOverwatchLevel
    global cachedOverwatchRank
    req = requests.get("https://playoverwatch.com/en-us/career/pc/us/Bonana-1364")
    doc = BeautifulSoup.BeautifulSoup(req.content)
    cachedOverwatchLevel = doc.find('div', {"class" : "player-level"}).div.contents[0]
    cachedOverwatchRank = doc.find('div', { "class" : "competitive-rank"}).div.contents[0]

def set_realm_stats():
    req = requests.get('https://www.realmeye.com/player/Bonanah')
    doc = BeautifulSoup.BeautifulSoup(req.content)
    cachedRealmStats = doc.find('span', { "class" : "player-stats" })

    req = requests.get('https://www.realmeye.com/pets-of/Bonanah')
    doc = BeautifulSoup.BeautifulSoup(req.content)
    cachedPetStats = doc.find('table', { "class" : "table table-striped tablesorter" })

def get_pet_stats():
    cachedPetStats = cachedPetStats.contents[1].contents[0]
    petName = str(cachedPetStats.contents[1])
    petName = petName[4 : len(petName) - 5]
    rarity = str(cachedPetStats.contents[2])
    rarity = rarity[4 : len(rarity) - 5]

    skillOne = str(cachedPetStats.contents[5].contents)
    skillOne = skillOne[7 : len(skillOne) - 8]

    skillTwo = str(cachedPetStats.contents[7].contents)
    skillTwo = skillTwo[7 : len(skillTwo) - 8]

    skillThree = str(cachedPetStats.contents[9].contents)
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

        skillOneLevel = str(cachedPetStats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
    elif disabledText.match(skillThree):
        match = disabledText.match(skillThree)
        skillThree = skillThree[match.end():]
        
        skillOneLevel = str(cachedPetStats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
        skillTwoLevel = str(cachedPetStats.contents[8].contents[0].contents)
        skillTwoLevel = skillTwoLevel[3 : len(skillTwoLevel) - 2]
    else:
        skillOneLevel = str(cachedPetStats.contents[6].contents[0].contents)
        skillOneLevel = skillOneLevel[3 : len(skillOneLevel) - 2]
        skillTwoLevel = str(cachedPetStats.contents[8].contents[0].contents)
        skillTwoLevel = skillTwoLevel[3 : len(skillTwoLevel) - 2]
        skillThreeLevel = str(cachedPetStats.contents[10].contents[0].contents)
        skillThreeLevel = skillThreeLevel[3 : len(skillThreeLevel) - 2]

    print skillThreeLevel + ", " + skillThree;

    return "My {0} {1} has {2} {3}, {4} {5}, and {6} {7}".format(rarity, petName, skillOneLevel, skillOne, \
        skillTwoLevel, skillTwo, skillThreeLevel, skillThree)

def format_realm_stats():
    req = requests.get('https://www.realmeye.com/player/Bonanah')
    doc = BeautifulSoup.BeautifulSoup(req.content)
    cachedRealmStats = doc.find('span', { "class" : "player-stats" })

    baseStats = cachedRealmStats['data-stats']
    statBonuses = cachedRealmStats['data-bonuses']
    level = cachedRealmStats['data-level']
    if (cachedRealmStats == None or baseStats == None or statBonuses == None):
        return 'Cannot access RealmEye stats at this time FeelsBadMan'
    #convert strings into arrays
    baseStats = baseStats[1 : len(baseStats) - 1].split(',')
    statBonuses = statBonuses[1 : len(statBonuses) - 1].split(',')
    maxedStats = cachedRealmStats.contents[0]
    currentClass = cachedRealmStats.parent
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


def setup():
    set_overwatch_stats()
    set_realm_stats()
 
while True:
    readbuffer = readbuffer + s.recv(1024)
    temp = string.split(readbuffer, "\n")
    readbuffer = temp.pop()
 
    for line in temp:
        # Checks whether the message is PING because its a method of Twitch to check if you're afk
        if ("PING :tmi.twitch.tv" in line):
            s.send("PONG {0}{1}\r\n".format(line[1],pingNum))
            #print "ping {0}".format(pingNum)
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
                        if twitchJson['stream'] == None:
                            send_message("I am currently not online and therefore have no current game stats to give you")
                            print "I am currently not online and therefore have no current game stats to give you"
                        else:
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

                    if message.lower() == "!test":
                        get_overwatch_stats()

                for l in parts:
                    if "End of /NAMES list" in l:
                        MODT = True