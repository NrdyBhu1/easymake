#include "easyjson.h"
#include <stdio.h>

int main(void)
{
	String json = "{ \"project\": \"easymake\", \"targets\": [ \"debug\": { \"compiler\": \"gcc\" } ] }";
	
	JsonValue value = ezjson_compile(json);
	
}
