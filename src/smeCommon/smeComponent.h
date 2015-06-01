/*
 * smeComponent.h
 *
 *  Created on: Apr 25, 2015
 * by Mik (smkk@amel-tech.com)
 */

#ifndef SMECOMPONENT_H_
#define SMECOMPONENT_H_

class smeComponent {
public:
    smeComponent(const char *name);
    virtual ~smeComponent();
    virtual void begin(void) {
    }
    const char* getName(void) const {
        return _name;
    };

public:
    virtual char readSerialNumber(char serialNum[]);

private:
    char _name[10];
};

#endif /* SMECOMPONENT_H_ */
