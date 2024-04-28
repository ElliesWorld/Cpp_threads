# Cpp_threads

"A thread safe and synchronized producer-consumer program.

The producer is a thread and it manufactures vehicles and stores them in a warehouse.

Every vehicle has the following properties:

    ID : a unique auto-generated serial number greater than 1000

    Model: to specify model of vehicles

    Type : car or truck

Every car has a property to specify the max number of passengers.
Every truck has a property to specify the max load weight.

The warehouse is an uncopyable circular buffer class implemented using an array whose capacity is specified by a macro in the command line when you compile the program. The capacity shall be greater than 7.

A consumer is a thread. When a consumer gets a vehicle from the warehouse, it prints all the properties of the vehicle to the terminal. Number of consumers shall be at least 2 and it is specified as an argument when the program is run in the terminal.

The producer manufactures cars and trucks randomly and stores them into the warehouse. When the warehouse is full the manufacturing thread should be sleeping.

A consumer gets a vehicle at a time from the warehouse and prints all the properties of the vehicle to the terminal. When the warehouse is empty the consuming threads should sleep until a vehicle is manufactured by the manufacturer thread and stored in the warehouse."
