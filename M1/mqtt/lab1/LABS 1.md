# Summary:
```table-of-contents
title: 
style: nestedList # TOC style (nestedList|nestedOrderedList|inlineFirstLevel)
minLevel: 0 # Include headings from the specified level
maxLevel: 0 # Include headings up to the specified level
include: 
exclude: 
includeLinks: true # Make headings clickable
hideWhenEmpty: false # Hide TOC if no headings are found
debugInConsole: false # Print debug info in Obsidian console
```
# MQTT Protocol
## Pub-Sub
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

## Topic

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

### Data set to test
```bash
mosquitto_pub -t "home/kitchen/temperature" -m "25"
mosquitto_pub -t "home/room/temperature" -m "32"
mosquitto_pub -t "home/hall/light" -m "100"
mosquitto_pub -t "home/hall/temperature" -m "80"
```

## Shared-Server
> Read the whole page http://test.mosquitto.org. What is this about?

`test.mosquitto.org` is a publicly available, open-access test server (broker) for the **Eclipse Mosquitto** MQTT project. It exists so developers can test MQTT messaging code, connections, and client applications across various ports and encryption protocols without needing to host their own broker.

> `eduroam` filters MQTT packets, hence you need to use your phone as router for this exercice. If using a phone is not possible for you, skip this exercice (or use a colleague's phone).
> Listen on all topics starting with `irco/`. Send a text using `irco/yourname`. Check that you receive it, other colleagues receive it too, and that you receive other colleagues' texts.

```bash
mosquitto_sub -h test.mosquitto.org -p 1883 -t irco/# -v
mosquitto_pub -r -h test.mosquitto.org -p 1883 -t irco/moon -m "Moon"
```

> What risk does this exercice introduce on your laptop (if you listen on # for ex.), from security point of view?

Privacy Leakage:
- the `#` captures every unencrypted messages published to the server.
Denial of Service:
- `test.mosquitto.org` receives a huge volume of traffic from users around the globe so the sub machine if listening to every topic
Code Injection:
- might execute javascript code (RCE)

# RabbitMQ broker and clients

> Install `nmap`, `wireshark`, and `rabbitmq-server` Debian/Ubuntu packages. Write down all the binary programs provided by `rabbitmq-server` package.
> Find out the programs allowing to start the RabbitMQ broker. Start it.

```bash
sudo systemctl start rabbitmq
sudo systemctl enable rabbitmq # started at system launched
```

> Write down the port(s) RabbitMQ server listens on.

**port 5672** for standard AMQP client connections
**port 15672** for the Management Plugin HTTP API. 

> Find out the commands (program names) allowing to create queue, send and receive a message



# RabbitMQ and AMQP

> Install `amqp-tools` package. Create a queue. Send a message and get it. Capture with wireshark the packet sent with the message, and the packet to get that message. Is it simple to understand?

![[Pasted image 20260925160514.png]]
![[Pasted image 20260925160503.png]]

# RabbitMQ and STOMP, transaction

```bash
# to use STOMP, listen to port 61613 (or 61614 for TLS)
nc localhost 61613
```

Exo:
- clients A and B subscribe to some destination
- client C sends a message to that destination
- check that the message is shown by clients A and B
- client A unsubscribes from the destination
- client C sends a second message to the destination
- check that the message is not shown by client A, but is shown by client B

```bash
# for client-a, client-b and client-c
CONNECT

^@
```

```bash
# client-a
SUBSCRIBE
id:sub-a
destination:/topic/test

^@

# client-b
SUBSCRIBE
id:sub-b
destination:/topic/test

^@
```

```bash
# client-c
SEND
destination:/topic/test

Hello Test
^@
```

```bash
# unsub client-a
UNSUBSCRIBE
destination:/topic/test

^@
```

```bash
# client-c
SEND
destination:/topic/test

Hello sub-b
^@
```

# AMQP and STOMP interoperability

> Create a STOMP subscriber to some queue/destination/topic. Create an AMQP sender to that queue. Check that the subscriber received the message.
> Where is the message queue, i.e. where are the messages stored waiting to be read?
> Create an AMQP subscriber to the same destination. Send to it a message. Check that both subscribers receive the message.

```bash
nc localhost 61613

# creating the STOMP subscriber to /topic/test
CONNECT 
host:/

^@
CONNECTED
server:RabbitMQ/3.9.27
session:session-BiNrJ6OyRkrAPHsJgHjRyg
heart-beat:0,0
version:1.0


SUBSCRIBE
id:sub-a   
destination:/topic/test

^@
```

```bash
# sending message through AMQP broker to /topic/test
amqp-publish -e amq.topic -r test -b "Hello from AMQP"
```

```bash

MESSAGE
subscription:sub-a
destination:/topic/test
message-id:T_sub-a@@session-BiNrJ6OyRkrAPHsJgHjRyg@@1
redelivered:false
persistent:1
content-length:15

Hello from AMQP
```

```bash
# creating the AMQP subscriber to /topic/test
amqp-consume -e amq.topic -r test cat
```

The message queue is maintained entirely inside the **RabbitMQ broker core (Erlang process).**

- When a STOMP subscriber connects to `/topic/my_topic`, RabbitMQ creates an **unnamed, temporary, auto-delete Erlang queue** behind the scenes.  
- It binds this queue to the standard AMQP exchange **`amq.topic`** using the routing key `my_topic`.
- The broker stores messages in this queue in RAM/disk until delivered to active consumers.

# ActiveMQ broker

> Stop or uninstall RabbitMQ. Check that it does not listen anymore. Install ActiveMQ broker and check that it is started. Write down all the binary programs provided by the package.
> Enable MQTT and STOMP protocol support, see https://activemq.apache.org/components/classic/documentation/mqtt and https://activemq.apache.org/components/classic/documentation/stomp. (AMQP does not work.)
> Check with `nmap` that it is listening on MQTT and STOMP ports.
> Start a STOMP subscriber and an MQTT subscriber. Send a message with STOMP and check that both subscribers receive it. What do you conclude?

```bash
sudo systemctl stop rabbitmq-server 
sudo systemctl disable rabbitmq-server
sudo systemctl status rabbitmq-server # should returned inactive
```

```bash
sudo ss -tulpn | grep -E '5672|15672|25672'
```

```bash
sudo apt-get update && sudo apt-get install -y activemq
```

```bash
sudo systemctl start activemq
# check if it's running
sudo systemctl status activemq
```

```xml
<!-- /etc/activemq/instances-enabled/main/activemq.xml -->
<!-- edit xml config to allow stomp and mqtt connection -->
<transportConnectors>
    <!-- OpenWire (Default) -->
    <transportConnector name="openwire" uri="tcp://0.0.0.0:61616?maximumConnections=1000&amp;wireFormat.maxFrameSize=104857600"/>

    <!-- Enabled STOMP (Default port: 61613) -->
    <transportConnector name="stomp" uri="stomp://0.0.0.0:61613?maximumConnections=1000&amp;wireFormat.maxFrameSize=104857600"/>

    <!-- Enabled MQTT (Default port: 1883) -->
    <transportConnector name="mqtt" uri="mqtt://0.0.0.0:1883?maximumConnections=1000&amp;wireFormat.maxFrameSize=104857600"/>
</transportConnectors>
```