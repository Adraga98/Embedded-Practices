#ifndef ETHERNET_CFG_H
#define ETHERNET_CFG_H

#define NUM_PHRASES 16

#define MAC_ADDRESS_src                        \
    {                                      \
        0x54, 0x27, 0x8d, 0x00, 0x00, 0x00 \
    }
#define MAC_ADDRESS_dest                       \
    {                                      \
        0x70, 0xC9, 0x4E, 0x1D, 0xFC, 0x0A \
    }

typedef struct {
	uint8_t macAddress_dest[6];
	uint8_t macAddress_src[6];
}ETHcfg;

const char *phrase[NUM_PHRASES] = {
	"No todo lo que es oro reluce...",
	"Aun en la oscuridad...",
	"¿Qué es la vida?",
	"No temas a la ocuridad",
	"Hasta los más pequeños",
	"No digas que el sol se ha puesto",
	"El coraje se encuentra",
	"No todos los tesoros",
	"Es peligroso",
	"Un mago nunca llega tarde",
	"Aun hay esperanza",
	"El mundo esta cambiando",
	"Las raices profundas",
	"No se puede",
	"Y sobre todo",
	"De las cenizas, un fuego"
};

#endif //ETHERNET_CFG_H
