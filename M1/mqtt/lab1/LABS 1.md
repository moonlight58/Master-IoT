# Summary:
[Pub/Sub](#pub-sub)

[Topic](#topic)

[Shared Server](#shared-server)

# Pub-Sub
> Q1. Send a message of some topic using a publisher, afterwards start a subscriber of that topic, and finally send another message of the same topic. Does the subscriber show both messages or not?

The subscriber does not show both messages if you forget about the retain flag `-r`.
It does show both messages if i'm not dumb

```bash
$ mosquitto_pub -h localhost -r -t lab/topic -m "test"
$ mosquitto_sub -h localhost -t lab/topic
```

> Wireshark listening for Publishers
> **MQTT Type:** Publish Message
> **QoS Level:** 0
![[Pasted image 20260910145341.png]]

> Wireshark listening for Subscribers. 
> **MQTT Type:** Subscriber Request
> **QoS Level:** 0
![[Pasted image 20260910145735.png]]

# Topic

> Create the following topic publishers: `home/x/y`, with x = {kitchen, hall, room} and y = {temperature, light, batterypercentage}.

> Create a client which monitors all the sensors of the kitchen, echoing them each time they change in the following precise format:
> - when publisher is executed with "home/hall/humidity 70" it shows:  
    `hh:mm:ss hall, humidity is 70`
> - when publisher is executed with "home/kitchen/temperature 30" it shows:  
    `hh:mm:ss kitchen, temperature is 30`
> - additionally, when temperature exceeds 30° it shows the message:  
    `Emergency in ...: current temperature is ...`

```bash
mosquitto_sub -v -t "home/+/+" | awk '{
    split($1, t, "/");
    room = t[2]; sensor = t[3]; value = $2;
    ts = strftime(%"%H:%M:%S%");
    printf "%s %s, %s is %s\n", ts, room, sensor, value;
    if (sensor == "temperature" && value+0 > 30) {
        printf "Emergency in %s: current temperature is %s\n", room, value;
    }
    fflush();
}'
```

## Data set to test
```bash
mosquitto_pub -t "home/kitchen/temperature" -m "25"
mosquitto_pub -t "home/room/temperature" -m "32"
mosquitto_pub -t "home/hall/light" -m "100"
mosquitto_pub -t "home/hall/temperature" -m "80"
```

# Shared-Server
