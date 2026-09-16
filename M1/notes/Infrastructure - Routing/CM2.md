# MQTT Topics
- Recall taht in pub-sub model, publishers categorise their messages into topics
- topices are utf-8 strings, with one or more topics separated by "/", thus creating a hierarchy
- + wildcard in subscription matches any string for a single topic at that position
- \# wildcard as last character in subscription matches any string for zero or more topic levels
- Brokers automatically create the prefix \#P2P/ for each client, which enables messages to be sent directly to that client (for example, in request/reply scenarios)

# MQTT Layer
- At application layer, on top of TCP/IP, port 1833 (8333 if over TLS)
	- MQTT-SN (for sensor networks) is a variation aimed on embedded systems using Zigbee or Bluetooth

# MQTT messages types
- Connect - waits for a connection to be establisehd with the server and creates a link between the nodes
- Disconnect - waits for the MQTT client to finish any work is must do, and for the TCP/IP session to disconnect
- Publish - returns immediately to the application thread after passing the request to the MQTT client
- ...

# MQTT-packet-types
- Reserved
- CONNECT, CONNACK
- PUBLISH, PUBACK, PUBREC, PUBREL, PUBCOMP
- SUSBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK
- PINGREQ, PINGRESP
- DISCONNECT
- AUTH

# MQTT control pacet format
- All MQTT packets are control packets, which have:
- fixed header, mandatory
- variabled ...

# MQTT Security
- MQTT sens connection credentials in plain text format and does not include any security measures
- Security can be provided by the underlying TCP transport (TLS for ex.)

# MQTT-QoS
- Each connection tot the broker can specify a QoS:
	- level 0, at most once, the message is sent only once and the receiver takes no additional steps to acknowledge delivery (fire and forget)
	- level 1, at least once, the message is re-tried by the sender multiple times until acknowledgement is received (acknowledged delivery)
	- level 2, exactly once, the sender and receiver engage into a communication taht ensures taht exactly one copy of the message is received (assured delivery)

# MQTT Applications
- Facebook has used aspects of MQTT, but "it is unclear how much and for what"
- Amazon Web SErvices announces something (check prof slides)

# MQTT - Questions

- read https://en.wikipedia.org/wiki/MQTT 
- What OSI layer does MQTT work at? How many QoS types does MQTT provide? 
	- MQTT works on the Application Layer.
	- [3 level of QoS](#mqtt-qos)
- What is shared subscription, a feature of MQTT v5.0? 
	- a shared subscription allow the load to be balanced across clients, thus reducing the risk of load problems.
	  It means there are 20 subscribers that are sub to a particular topic and they're grouped together. Another 20 subscribers also sub to this particular topic and also grouped together. Thus making only 2 groups and the broker sends only 2 messages instead of a total of 40.
- How many message types does MQTT have? What is the purpose of SUBACK message type and what does it mean if the first byte (bit?) of its payload is 1?
	- [15 types of messages](#mqtt-packet-types)
	- broker confirm receipt of a **SUBSCRIBE** request to client and specify the [granted level of QoS](https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html#_Toc3901178).

