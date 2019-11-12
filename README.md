# StreamBase

Clients communicate with the server by initially sending a header message containing an Action which specifies whether they want to do a save or a get and what key to use.

## Send
On receiving a send action, the server will expect and read a second message containing the binary archive of the data.
After inserting the data into the store, the server will send a message back to the client notifying it the save has been completed.

## Get
On receiving a get action, the server will retrieve the binary archive for the given key from the store and send it back in a message to the client.
Note that the client-side `get()` function requires the type of the data being returned to be specified.

## Data Store
The server stores data in a map from the given key to a binary archive of the provided object.
The map is guarded by a shared mutex to ensure thread-safe access.

## Serialization

This implementation of StreamBase depends on `cereal` to serialize data.
This means extra code may be required for certain types of data
e.g. for custom classes a `serialize` method that is a `friend` of the class may be required in order to serialize its attributes, as C++ lacks reflection.

## Assumptions

For simplicity it is assumed data being sent fits within a single message, and that cases will conform to the spec meaning there is currently no error handling
e.g. attempting to retrieve data which has not been saved is unhandled, clients expect the server to be running, etc.

## Multithreading

This implementation of StreamBase supports async via multithreading. The server uses a different thread and pipe instance for each client connection.

Multithreaded code uses a custom `log()` function as `cout` is not thread-safe.
