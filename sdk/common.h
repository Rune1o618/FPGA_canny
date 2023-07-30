/*
 * common.h
 *
 *  Created on: 07.01.2023
 *      Author: wszcz
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

struct pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    //uint8_t alpha;
} __attribute__((__packed__));

#endif /* SRC_COMMON_H_ */
