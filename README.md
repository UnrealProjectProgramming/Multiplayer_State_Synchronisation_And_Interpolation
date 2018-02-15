# Multiplayer_State_Synchronisation_And_Interpolation\


* AutonomousProxy vs SimulatedProxy  https://docs.unrealengine.com/latest/INT/Gameplay/Networking/Actors/Roles/


* Replicating variables from the server: https://docs.unrealengine.com/latest/INT/Gameplay/Networking/Actors/Properties/    we are overriding the variables from a server prespective.

* Understanding the concept of lag and glitching and How to simulate them in unreal engine using the command lines " ~ "  .
  https://docs.google.com/presentation/d/13v56Mb-Moefts1VOQLBq-5_d-qGs2ny90QpIXgdNd8Q/edit#slide=id.gf8b855009_5_1
  https://www.unrealengine.com/en-US/blog/finding-network-based-exploits

* Comparing the methods we used so far , in method one: 
  Problem : Not smooth
  Information Sent to server : throw
  between updates: we do nothing so we can only see the movment from the "Server , Authority" prespective but we can't see it in the Autonomous and Simulated Proxies.
  Information recived: Transform and velocity.
  OnRecipet: Overwrite Local reviced info.


  In Method two:
  Problem : LAG
  Information Sent to server : throw
  between updates: Simulating
  Information recived: Transform and velocity.
  OnRecipet: Overwrite Local reviced info.


  In Method 3:
  Problem : ....
  Information Sent to server : throw
  between updates: Simulating
  Information recived: Transform and velocity and server time.
  OnRecipet: Replay the control since server time , meaning that it will go back to the previous state in this Exact Time.
