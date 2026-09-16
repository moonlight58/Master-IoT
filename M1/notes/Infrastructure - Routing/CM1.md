Internet of object -> called internet because there's 23.9b devices.
More than computers ~5b

internet comes from "**inter** connect **net**work"

# Useful features of an IoT protocol
- Provide the communication pattern needed by the application (described later)
- Simplicity (cf. STOMP)
- Feature-richness (cf. AMQP)
- Popularity (cf. MQTT)
- Specific functions in IoT: device data collection
- ...

# Communication models/patterns

## Request-Response pattern
- One-to-one: a machine asks another machine, which answer to it.
- Allows machine to get informations in real time from another machine, like HTTP, RMI, RPC
- Properties:
	- receiver needs to be alive (like phone call not like SMS/email)
	- tight coupling: client must know the server address and both need to discuss the same language
	- sender needs to wait the response. If too long, possible to return partial results to show progress
## Asynchronous Messaging patterns
- Also knows as 'fire-and-forget'
- Loosing coupling: the sender puts a message in message queue and does not require an immediate response to continue processing, like e-mail
- Queues retain all messages sent to them until the messages are consumed or expire
- Request-response model need that both parties be available, whereas queuing takes the message and sends it to destination when it becomes available
- need to know the destination

- **Dead letter queue**
	- the destination queue does not exist
	- the queue length limit is exceeded
	- message length limit is exceeded
- Allows dev to looks for common patterns and potential software problems.

## Publish-Subscribe pattern
- this pattern consists of clients communicating with a server
	- broker = the one who transacts business for another
	- subscribers subscribe to broker to one topics
	- the publishers send to broker data
	- the broker transmits to subscribers
	- e.g. a smartphone which receives data from all brightness sensors
- One-to-many: allows efficient mass distribution of data to multiple consumers, allowing system monitoring
- Properties:
	- indirect comm
	- asynchronous
	- works in heterogeneous platforms, easier to change or update
- Clients only interact with a broker, but a system may contain several brokers

## Fan out pattern
"to spread out over a wide area"
- a list of tasks and several processors/machines
- The message is delivered to one worker only in a round-robin fashion
- Analogy with firemen: when someone informs the center about a fire, the broker sens this information to a car; when another fire occurs, the server selects another car; and so on
- Fan-in allows to collect the results, if appropriate
- Pub/sub is used for wide message distribution (data diffusion), whereas fan-out is more about resource allocation
## Router-Dealer pattern
- A server (the market) and clients (dealers/traders)
	- traders place buy and sell orders by sending relevant 'order' messages to the market
	- the market responds immediately with an 'acknowlegde' message
	- sometime later when an order is fulfilled the market sends 'order complete' messages to all involved
- responses are synchronous
- each subscriber is identified uniquely through an id, and servers answers to specific clients