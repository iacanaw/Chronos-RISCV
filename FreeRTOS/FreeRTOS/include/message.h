#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_SIZE 128

typedef struct {
    volatile unsigned int length;
    volatile unsigned int msg[ MSG_SIZE ];
} Message;

#endif /* __MESSAGE_H__ */
