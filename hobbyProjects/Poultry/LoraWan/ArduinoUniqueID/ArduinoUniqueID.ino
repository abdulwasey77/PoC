//
// ArduinoUniqueID.ino
//
// Example shows the UniqueID on the Serial Monitor.
//

#include <ArduinoUniqueID.h>

void setup()
{
	Serial.begin(115200);
//	UniqueIDdump(Serial);
	Serial.print("UniqueID: ");
 String id;
	for (size_t i = 0; i < UniqueIDsize; i++)
	{
    id+=String(UniqueID[i]);
		
	}
	Serial.println(id);
}

void loop()
{
}
