/*
 * smeComponent.cpp
 *
 *  Created on: Apr 25, 2015
 * by Mik (smkk@amel-tech.com)
 */

#include "smeComponent.h"
#include <string.h>

smeComponent::smeComponent(const char *name) {
    memcpy(_name, name , strlen(name));
}

smeComponent::~smeComponent() {
    // TODO Auto-generated destructor stub
}


char smeComponent::readSerialNumber(char serialNum[]){
	return 0;
}
