uses lora to extend a Master control link bus wirelessy 

//connect datalink to pin 3 directly, this is the "sense pin" 

//connect datalink also to pin 6 this is the "talk" pin 
          //case (A) arduino is the slave. 
          if you are connecting to a beomaster, beocenter, beosound or similar this pin will contoll a transistor that will pull the datalink to ground.
          in this case the moteino is mounted on the back of the beomaster as it broadcasts what he beomaster sends on the buss as well as reciving messages from the toher wireless devices
          
          //case (B) arduino is the master. if you are connected to a slave device, such as a master contorl link, the arduino will provide the voltage for the datalink bus, and the slave device will pull it to gournd.
          for this use case connect pin 6 to datalink trough a 10k resistor
          dont forghet to connect ground
          
          in this case the moteino is connected to the infrared reciver of a Master Control Link 2 - MCL2 box 
